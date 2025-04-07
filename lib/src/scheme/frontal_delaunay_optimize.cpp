// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// Orignal file: meshGFaceOptimize.cpp

#include <stack>
#include "krado/background_mesh_tools.h"
#include "krado/mesh_element.h"
#include "krado/scheme/frontal_delaunay_optimize.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/quality_measures.h"
#include "krado/mesh_surface.h"
#include "krado/geom_surface.h"
#include "krado/vector.h"
#include "krado/numerics.h"
#include "krado/log.h"

namespace krado {

void
reverse_triangle(MeshElement & tri)
{
    assert(tri.type() == ElementType::TRI3);
    tri.swap_vertices(1, 2);
}

static void
set_lcs_init(const MeshElement & t, std::map<MeshVertexAbstract *, double> & vSizes)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            auto * vi = t.vertex(i);
            auto * vj = t.vertex(j);
            assert(vi != nullptr);
            assert(vj != nullptr);
            vSizes[vi] = -1;
            vSizes[vj] = -1;
        }
    }
}

static void
set_lcs(const MeshElement & t,
        std::map<MeshVertexAbstract *, double> & vSizes,
        BidimMeshData & data)
{
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            auto * vi = t.vertex(i);
            auto * vj = t.vertex(j);
            if (vi != data.equivalent(vj) && vj != data.equivalent(vi)) {
                assert(vi != nullptr);
                assert(vj != nullptr);
                auto d = vi->point() - vj->point();
                auto l = d.magnitude();

                auto iti = vSizes.find(vi);
                auto itj = vSizes.find(vj);
                if (iti->second < 0 || iti->second > l)
                    iti->second = l;
                if (itj->second < 0 || itj->second > l)
                    itj->second = l;
            }
        }
    }
}

bool
build_mesh_generation_data_structures(MeshSurface & msurface,
                                      std::set<MTri3 *, MTri3::ComparePtr> & all_tris,
                                      BidimMeshData & data)
{
    std::map<MeshVertexAbstract *, double> v_sizes_map;

    for (auto & tri : msurface.triangles())
        set_lcs_init(tri, v_sizes_map);

    if (v_sizes_map.find(nullptr) != v_sizes_map.end()) {
        Log::error("Some NULL points exist in 2D mesh");
        return false;
    }

    for (auto & tri : msurface.triangles())
        set_lcs(tri, v_sizes_map, data);

    // take care of embedded vertices
    std::set<MeshVertexAbstract *> embedded_vertices;
    for (auto & mvtx : msurface.embedded_vertices()) {
        if (mvtx) {
            v_sizes_map[mvtx] = std::min(v_sizes_map[mvtx], mvtx->mesh_size());
            embedded_vertices.insert(mvtx);
        }
    }

    // take good care of embedded edges
    for (auto & edg : msurface.embedded_curves()) {
        assert(edg != nullptr);
        if (!edg->is_mesh_degenerated()) {
            for (auto & seg : edg->segments()) {
                assert(seg.type() == ElementType::LINE2);
                data.internal_edges.insert(seg);
            }
        }
    }

    // take care of small edges in  order not to "pollute" the size field
    for (auto & mcrv : msurface.curves()) {
        assert(mcrv != nullptr);
        if (!mcrv->is_mesh_degenerated()) {
            for (auto & seg : mcrv->segments()) {
                auto * v1 = seg.vertex(0);
                auto * v2 = seg.vertex(1);
                assert(v1 != nullptr);
                assert(v2 != nullptr);
                auto d = utils::distance(v1->point(), v2->point());
                auto d0 = v_sizes_map[v1];
                auto d1 = v_sizes_map[v2];
                if (d0 < .5 * d)
                    v_sizes_map[v1] = .5 * d;
                if (d1 < .5 * d)
                    v_sizes_map[v2] = .5 * d;
            }
        }
    }

    for (auto & [vtx, size] : v_sizes_map) {
        auto [uv, b] = reparam_mesh_vertex_on_surface(vtx, msurface.geom_surface());
        // auto pt = vtx->point();
        // Add size of background mesh to embedded vertices. For the other nodes,
        // use the size in vSizesMap
        // FIXME: const double lcBGM = (embeddedVertices.count(it->first) > 0)
        // FIXME:                          ? BGM_MeshSize(surface, u, v, pt.x, pt.y, pt.z)
        // FIXME:                          : it->second;
        const double lc_bgm = size;
        data.add_vertex(vtx, uv, size, lc_bgm);
    }
    for (auto & tri : msurface.triangles()) {
        std::array<int, 3> idx = { data.index(tri.vertex(0)),
                                   data.index(tri.vertex(1)),
                                   data.index(tri.vertex(2)) };
        double lc =
            0.3333333333 * (data.v_sizes[idx[0]] + data.v_sizes[idx[1]] + data.v_sizes[idx[2]]);
        double lc_bgm = 0.3333333333 * (data.v_sizes_bgm[idx[0]] + data.v_sizes_bgm[idx[1]] +
                                        data.v_sizes_bgm[idx[2]]);

        auto ll = extend_1d_mesh_in_2d_surfaces(msurface) ? std::min(lc, lc_bgm) : lc_bgm;
        all_tris.insert(new MTri3(tri, ll, &data, &msurface));
    }
    msurface.remove_all_triangles();
    connect_triangles(all_tris);

    return true;
}

void
compute_equivalences(MeshSurface & msurface, BidimMeshData & data)
{
    if (data.equivalence) {
        std::vector<MeshElement> new_tris;
        for (auto & t : msurface.triangles()) {
            std::array<MeshVertexAbstract *, 3> tri { nullptr, nullptr, nullptr };
            for (int j = 0; j < 3; ++j) {
                tri[j] = t.vertex(j);
                auto it = data.equivalence->find(tri[j]);
                if (it != data.equivalence->end())
                    tri[j] = it->second;
            }
            if (tri[0] != tri[1] && tri[0] != tri[2] && tri[2] != tri[1])
                new_tris.emplace_back(MeshElement::Tri3(tri));
        }
        msurface.set_triangles(new_tris);
    }
}

struct EquivalentTriangle {
    MeshElement t;
    MeshVertexAbstract * v[3];
    EquivalentTriangle(const MeshElement & t,
                       std::map<MeshVertexAbstract *, MeshVertexAbstract *> * equivalence) :
        t(t)
    {
        assert(equivalence != nullptr);
        for (int i = 0; i < 3; ++i) {
            auto * vtx = t.vertex(i);
            auto it = equivalence->find(vtx);
            if (it == equivalence->end())
                this->v[i] = vtx;
            else
                this->v[i] = it->second;
        }
        std::sort(this->v, this->v + 3);
    }

    bool
    operator<(const EquivalentTriangle & other) const
    {
        for (int i = 0; i < 3; ++i) {
            if (other.v[i] > v[i])
                return true;
            if (other.v[i] < v[i])
                return false;
        }
        return false;
    }
};

bool
compute_equivalent_triangles(const MeshSurface & msurface,
                             std::map<MeshVertexAbstract *, MeshVertexAbstract *> * equivalence)
{
    if (equivalence == nullptr)
        return false;
    std::vector<MeshElement> wtf;
    std::set<EquivalentTriangle> eq_tris;
    for (auto & tri : msurface.triangles()) {
        EquivalentTriangle et(tri, equivalence);
        auto iteq = eq_tris.find(et);
        if (iteq == eq_tris.end())
            eq_tris.insert(et);
        else {
            wtf.push_back(iteq->t);
            wtf.push_back(tri);
        }
    }

    if (wtf.size()) {
        Log::info("{} triangles are equivalent", wtf.size());
        return true;
    }
    return false;
}

void
split_equivalent_triangles(MeshSurface & msurface, BidimMeshData & data)
{
    compute_equivalent_triangles(msurface, data.equivalence);
}

void
transfer_data_structure(MeshSurface & msurface,
                        std::set<MTri3 *, MTri3::ComparePtr> & all_tris,
                        BidimMeshData & data)
{
    while (!all_tris.empty()) {
        auto it = all_tris.begin();
        MTri3 * worst = *all_tris.begin();
        assert(worst != nullptr);
        if (!worst->is_deleted()) {
            auto & elem = worst->tri();
            msurface.add_triangle({ elem.vertex(0), elem.vertex(1), elem.vertex(2) });
        }
        delete worst;
        all_tris.erase(it);
    }

    // make sure all the triangles are oriented in the same way in
    // parameter space (it would be nicer to change the actual algorithm
    // to ensure that we create correctly-oriented triangles in the
    // first place)

    // if BL triangles are considered, then all that is WRONG !

    if (msurface.triangles().size() > 1) {
        bool BL = false; //! gf->getColumns()->_toFirst.empty();

        Vector n1, n2;
        auto t = msurface.triangles()[0];
        auto *v0 = t.vertex(0), *v1 = t.vertex(1), *v2 = t.vertex(2);

        if (!BL) {
            auto index0 = data.index(v0);
            auto index1 = data.index(v1);
            auto index2 = data.index(v2);
            n1 = normal3points(Point(data.uv[index0]),
                               Point(data.uv[index1]),
                               Point(data.uv[index2]));
        }
        else {
            // BL --> PLANAR FACES !!!
            assert(v0 != nullptr);
            assert(v1 != nullptr);
            assert(v2 != nullptr);
            n1 = normal3points(v0->point(), v1->point(), v2->point());
        }
        for (auto & t : msurface.triangles()) {
            v0 = t.vertex(0);
            v1 = t.vertex(1);
            v2 = t.vertex(2);
            if (!BL) {
                auto index0 = data.index(v0);
                auto index1 = data.index(v1);
                auto index2 = data.index(v2);
                n2 = normal3points(Point(data.uv[index0]),
                                   Point(data.uv[index1]),
                                   Point(data.uv[index2]));
            }
            else {
                // BL --> PLANAR FACES !!!
                assert(v0 != nullptr);
                assert(v1 != nullptr);
                assert(v2 != nullptr);
                n2 = normal3points(v0->point(), v1->point(), v2->point());
            }
            // orient the bignou
            if (dot_product(n1, n2) < 0.0)
                reverse_triangle(t);
        }
    }
    split_equivalent_triangles(msurface, data);
    compute_equivalences(msurface, data);
}

} // namespace krado
