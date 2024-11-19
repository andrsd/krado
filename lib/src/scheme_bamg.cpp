// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme_bamg.h"
#include "krado/mesh.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/surface_index_mapper.h"
#include "bamg/bamglib/Mesh2.h"
#include "Eigen/Eigen"
#include <array>
#include <map>

namespace krado {

namespace {

static const bamg::Direction NoDirOfSearch = bamg::Direction();

class Bamg {
public:
    Bamg(const MeshSurface & surface) :
        surface(surface),
        Hmin(std::numeric_limits<bamg::Real8>::max()),
        len(nullptr),
        Th(nullptr),
        reft(nullptr)
    {
        this->Gh.EmptyGeometry();
        build_local_vertex_map();
        transfer_vertices();
        init_len();
        transfer_edges();
        initialize_vertex_metrics();
        destroy_len();
        this->Gh.AfterRead();
    }

    ~Bamg()
    {
        delete[] this->len;
        delete[] this->reft;
        delete this->Th;
    }

    void
    set_uniform_mesh_size(double h)
    {
        for (int i = 0; i < this->Gh.nbv; i++)
            this->Gh.vertices[i].m = bamg::Metric(h);
    }

    void
    triangularize(int nbvx = 50000)
    {
        this->Th = new bamg::Triangles(nbvx, this->Gh);
        this->reft = new bamg::Int4[Th->nbt];
        this->nbInT = this->Th->ConsRefTriangle(this->reft);
    }

    int
    num_of_triangles()
    {
        return this->Th->nbt;
    }

    bool
    is_triangle_active(int idx) const
    {
        return this->reft[idx] >= 0;
    }

    const bamg::Triangle &
    triangle(int idx) const
    {
        return this->Th->triangles[idx];
    }

    int
    num_of_edges() const
    {
        return this->Th->nbe;
    }

    const bamg::Edge &
    edge(int idx) const
    {
        return this->Th->edges[idx];
    }

private:
    void
    build_local_vertex_map()
    {
        this->local_vtx_id.clear();
        for (auto & curve : this->surface.curves()) {
            for (auto & vtx : curve->all_vertices()) {
                int id = this->local_vtx_id.size();
                this->local_vtx_id.try_emplace(vtx, id);
            }
        }
    }

    void
    transfer_vertices()
    {
        this->Gh.nbv = this->local_vtx_id.size();
        this->Gh.nbvx = Gh.nbv;
        this->Gh.vertices = new bamg::GeometricalVertex[this->Gh.nbv];
        this->Gh.nbiv = this->Gh.nbv;
        for (auto & [vtx, id] : this->local_vtx_id) {
            auto pt = vtx->point();
            this->Gh.vertices[id].r.x = pt.x;
            this->Gh.vertices[id].r.y = pt.y;
            this->Gh.vertices[id].ReferenceNumber = id;
            this->Gh.vertices[id].DirOfSearch = NoDirOfSearch;
            this->Gh.vertices[id].color = 0;
            this->Gh.vertices[id].Set();
            this->Gh.vertices[id].SetRequired();
        }
    }

    int
    get_number_of_edges()
    {
        int n_edges = 0;
        for (auto & curve : this->surface.curves()) {
            if (curve->is_meshed())
                n_edges += curve->segments().size();
            else
                n_edges++;
        }
        return n_edges;
    }

    void
    init_len()
    {
        this->len = new bamg::Real4[this->Gh.nbv];
        for (int i = 0; i < this->Gh.nbv; i++)
            this->len[i] = 0;
    }

    void
    transfer_edges()
    {
        this->Gh.nbe = get_number_of_edges();
        this->Gh.edges = new bamg::GeometricalEdge[Gh.nbe];

        for (int curve_idx = 0, eidx = 0; curve_idx < this->surface.curves().size(); curve_idx++) {
            auto & curve = this->surface.curves()[curve_idx];
            if (curve->is_meshed()) {
                auto & curve_vtxs = curve->all_vertices();
                for (auto & segs : curve->segments()) {
                    auto vtx1 = this->local_vtx_id[curve_vtxs[segs.ids()[0]]];
                    auto vtx2 = this->local_vtx_id[curve_vtxs[segs.ids()[1]]];
                    std::array<int, 2> edge = { vtx1, vtx2 };
                    add_edge(eidx, edge, curve_idx, true);
                    eidx++;
                }
            }
            else {
                // add one straight edge
                auto & bnd_vtxs = curve->bounding_vertices();
                std::array<int, 2> edge = { this->local_vtx_id[bnd_vtxs[0]],
                                            this->local_vtx_id[bnd_vtxs[1]] };
                add_edge(eidx, edge, curve_idx, false);
                eidx++;
            }
        }
    }

    void
    add_edge(int edge_idx, const std::array<int, 2> & edge, int curve_id, bool req)
    {
        bamg::R2 zero2(0, 0);

        auto i1 = edge[0];
        auto i2 = edge[1];

        this->Gh.edges[edge_idx].ref = curve_id;
        this->Gh.edges[edge_idx].v[0] = this->Gh.vertices + i1;
        this->Gh.edges[edge_idx].v[1] = this->Gh.vertices + i2;

        bamg::R2 x12 = this->Gh.vertices[i2].r - this->Gh.vertices[i1].r;
        auto l12 = std::sqrt((x12, x12));
        this->Gh.edges[edge_idx].tg[0] = zero2;
        this->Gh.edges[edge_idx].tg[1] = zero2;
        this->Gh.edges[edge_idx].SensAdj[0] = this->Gh.edges[edge_idx].SensAdj[1] = -1;
        this->Gh.edges[edge_idx].Adj[0] = this->Gh.edges[edge_idx].Adj[1] = 0;
        this->Gh.edges[edge_idx].flag = 0;

        this->Gh.vertices[i1].color++;
        this->Gh.vertices[i2].color++;
        this->len[i1] += l12;
        this->len[i2] += l12;

        if (req)
            this->Gh.edges->SetRequired();

        Hmin = Min(Hmin, l12);
    }

    void
    initialize_vertex_metrics()
    {
        for (int i = 0; i < this->Gh.nbv; i++)
            if (this->Gh.vertices[i].color > 0)
                this->Gh.vertices[i].m =
                    bamg::Metric(this->len[i] / (bamg::Real4) this->Gh.vertices[i].color);
            else
                this->Gh.vertices[i].m = bamg::Metric(this->Hmin);
    }

    void
    destroy_len()
    {
        delete[] this->len;
        this->len = nullptr;
    }

    const MeshSurface & surface;
    bamg::Geometry Gh;
    bamg::Real8 Hmin;
    std::map<MeshVertexAbstract *, int> local_vtx_id;
    bamg::Real4 * len;
    bamg::Triangles * Th;
    bamg::Int4 * reft;
    bamg::Int4 nbInT;
};

} // namespace

SchemeBAMG::SchemeBAMG() : Scheme("bamg") {}

void
SchemeBAMG::mesh_surface(MeshSurface & surface)
{
    Bamg mg(surface);
    mg.set_uniform_mesh_size(get<double>("max_area"));

    mg.triangularize();

    SurfaceIndexMapper im(surface);
    for (int i = 0; i < mg.num_of_edges(); i++) {
        auto & edge = mg.edge(i);
        auto crv_idx = edge.ref;
        auto curve = surface.curves()[crv_idx];
        curve->add_segment({ im.curve_idx(crv_idx, edge[0].r.x, edge[0].r.y),
                             im.curve_idx(crv_idx, edge[1].r.x, edge[1].r.y) });
    }

    for (int i = 0; i < mg.num_of_triangles(); i++) {
        if (mg.is_triangle_active(i)) {
            std::array<std::size_t, 3> tri;
            for (int j = 0; j < 3; j++) {
                auto vtx = mg.triangle(i)[j];
                auto idx = im.surface_idx(vtx.r.x, vtx.r.y);
                tri[j] = idx;
            }
            surface.add_triangle(tri);
        }
    }
}

} // namespace krado
