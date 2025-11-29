// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/triangle.h"
#include "krado/element.h"
#include "krado/mesh.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/surface_index_mapper.h"
#include <map>
#include <array>
#include <set>
#include <cassert>

#ifdef KRADO_WITH_TRIANGLE
    #include "krado/log.h"

extern "C" {
    #define REAL double
    #define VOID void
    #include "triangle.h"
}
#endif

namespace krado {

#ifdef KRADO_WITH_TRIANGLE

namespace {

class TriangleSession {
public:
    TriangleSession(Ptr<MeshSurface> surface, const SchemeTriangle::Options & opts) :
        surface_(surface),
        region_point_(opts.region_point),
        max_area_(opts.max_area)
    {
        init_io(this->in_);
        init_io(this->out_);
    }

    void
    create_pslg()
    {
        auto pt_id = build_point_map();
        create_point_list(pt_id);
        create_segment_list(pt_id);

        if (this->region_point_.has_value()) {
            this->in_.numberofregions = 1;
            this->in_.regionlist = new double[this->in_.numberofregions * 4];
            auto [x, y] = this->region_point_.value();
            this->in_.regionlist[0] = x;
            this->in_.regionlist[1] = y;
            this->in_.regionlist[2] = this->surface_->marker();
            this->in_.regionlist[3] = this->max_area_.value();
        }
    }

    void
    triangularize()
    {
        // p = triangulate planar straight line graph
        // z = zero-based indexing
        // Q = quiet
        auto switches = fmt::format("pzQ");
        if (!this->region_point_.has_value() && this->max_area_.has_value())
            switches += fmt::format("a{}", this->max_area_.value());
        else
            switches += fmt::format("a");
        triangulate((char *) switches.c_str(), &this->in_, &this->out_, nullptr);
    }

    int64
    n_triangles()
    {
        return this->out_.numberoftriangles;
    }

    Point
    point(int64 i, int64 j)
    {
        auto idx = this->out_.trianglelist[i * Tri3::N_VERTICES + j];
        double x = this->out_.pointlist[idx * 2 + 0];
        double y = this->out_.pointlist[idx * 2 + 1];
        return Point(x, y);
    }

    ~TriangleSession()
    {
        destroy_io(this->in_);
        // region list is shallow copied into `out`
        if (this->in_.regionlist)
            free(this->in_.regionlist);
        destroy_io(this->out_);
    }

private:
    void
    init_io(triangulateio & io)
    {
        io.pointlist = nullptr;
        io.pointattributelist = nullptr;
        io.pointmarkerlist = nullptr;
        io.numberofpoints = 0;
        io.numberofpointattributes = 0;
        io.trianglelist = nullptr;
        io.triangleattributelist = nullptr;
        io.trianglearealist = nullptr;
        io.neighborlist = nullptr;
        io.numberoftriangles = 0;
        io.numberofcorners = 0;
        io.numberoftriangleattributes = 0;
        io.segmentlist = nullptr;
        io.segmentmarkerlist = nullptr;
        io.numberofsegments = 0;
        io.holelist = nullptr;
        io.numberofholes = 0;
        io.regionlist = nullptr;
        io.numberofregions = 0;
        io.edgelist = nullptr;
        io.edgemarkerlist = nullptr;
        io.normlist = nullptr;
        io.numberofedges = 0;
    }

    std::map<Point, int64>
    build_point_map()
    {
        std::map<Point, int64> pt_id;
        auto & curves = this->surface_->curves();
        for (auto & c : curves) {
            auto & verts = c->all_vertices();
            for (auto & vtx : verts) {
                assert(vtx != nullptr);
                auto pt = vtx->point();
                auto id = pt_id.size();
                pt_id.try_emplace(pt, id);
            }
        }
        return pt_id;
    }

    void
    create_point_list(const std::map<Point, int64> & pt_id)
    {
        this->in_.numberofpoints = (int) pt_id.size();
        this->in_.pointlist = (double *) malloc(this->in_.numberofpoints * 2 * sizeof(double));
        for (auto & [pt, id] : pt_id) {
            int64 k = 2 * id;
            this->in_.pointlist[k + 0] = pt.x;
            this->in_.pointlist[k + 1] = pt.y;
        }
    }

    void
    create_segment_list(const std::map<Point, int64> & pt_id)
    {
        // segments
        int64 n_segments = 0;
        auto & curves = this->surface_->curves();
        for (auto & c : curves) {
            assert(c != nullptr);
            n_segments += (int64) c->segments().size();
        }
        this->in_.numberofsegments = n_segments;
        this->in_.segmentlist = (int *) malloc(n_segments * 2 * sizeof(int));

        int64 k = 0;
        for (auto & c : curves) {
            assert(c != nullptr);
            auto & segments = c->segments();
            for (auto & s : segments) {
                auto v1 = s.vertex(0);
                auto v2 = s.vertex(1);

                auto pt1 = v1->point();
                auto pt2 = v2->point();

                auto id1 = pt_id.at(pt1);
                auto id2 = pt_id.at(pt2);
                this->in_.segmentlist[k++] = id1;
                this->in_.segmentlist[k++] = id2;
            }
        }
    }

    void
    destroy_io(triangulateio & io)
    {
        if (io.pointlist)
            free(io.pointlist);
        if (io.pointattributelist)
            free(io.pointattributelist);
        if (io.pointmarkerlist)
            free(io.pointmarkerlist);
        if (io.trianglelist)
            free(io.trianglelist);
        if (io.triangleattributelist)
            free(io.triangleattributelist);
        if (io.trianglearealist)
            free(io.trianglearealist);
        if (io.neighborlist)
            free(io.neighborlist);
        if (io.segmentlist)
            free(io.segmentlist);
        if (io.segmentmarkerlist)
            free(io.segmentmarkerlist);
        if (io.holelist)
            free(io.holelist);
        if (io.edgelist)
            free(io.edgelist);
        if (io.edgemarkerlist)
            free(io.edgemarkerlist);
        if (io.normlist)
            free(io.normlist);
    }

    Ptr<MeshSurface> surface_;
    Optional<std::tuple<double, double>> region_point_;
    Optional<double> max_area_;
    triangulateio in_, out_;
};

} // namespace

#endif

void
SchemeTriangle::mesh_surface(Ptr<MeshSurface> surface)
{
#ifdef KRADO_WITH_TRIANGLE
    Log::info("Meshing surface {}: scheme='triangle'", surface->id());

    TriangleSession triangle(surface, this->opts_);
    triangle.create_pslg();
    triangle.triangularize();

    SurfaceIndexMapper im(surface);
    for (int64 i = 0; i < triangle.n_triangles(); i++) {
        std::array<Ptr<MeshVertexAbstract>, Tri3::N_VERTICES> tri;
        for (int j = 0; j < Tri3::N_VERTICES; j++) {
            auto pt = triangle.point(i, j);
            tri[j] = im.surface_vertex(pt.x, pt.y);
        }
        surface->add_triangle(tri);
    }

#else
    throw Exception("krado was not built with triangle support.");
#endif
}

SchemeTriangle::SchemeTriangle(Options options) : Scheme2D("triangle"), opts_(options) {}

} // namespace krado
