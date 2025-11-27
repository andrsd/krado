// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/triangle.h"
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

namespace tri {

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

std::map<Point, int>
build_point_map(Ptr<MeshSurface> surface)
{
    std::map<Point, int> pt_id;
    auto & curves = surface->curves();
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
create_point_list(const std::map<Point, int> & pt_id, triangulateio & io)
{
    io.numberofpoints = (int) pt_id.size();
    io.pointlist = (double *) malloc(io.numberofpoints * 2 * sizeof(double));
    for (auto & [pt, id] : pt_id) {
        int k = 2 * id;
        io.pointlist[k + 0] = pt.x;
        io.pointlist[k + 1] = pt.y;
    }
}

void
create_segment_list(Ptr<MeshSurface> surface,
                    const std::map<Point, int> & pt_id,
                    triangulateio & io)
{
    // segments
    int n_segments = 0;
    auto & curves = surface->curves();
    for (auto & c : curves) {
        assert(c != nullptr);
        n_segments += (int) c->segments().size();
    }
    io.numberofsegments = n_segments;
    io.segmentlist = (int *) malloc(n_segments * 2 * sizeof(int));

    int k = 0;
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
            io.segmentlist[k++] = id1;
            io.segmentlist[k++] = id2;
        }
    }
}

void
create_pslg(Ptr<MeshSurface> surface, const std::map<Point, int> & pt_id, triangulateio & io)
{
    create_point_list(pt_id, io);
    create_segment_list(surface, pt_id, io);
}

void
create_regions(Ptr<MeshSurface> surface, const SchemeTriangle::Options & opts, triangulateio & io)
{
    auto & scheme = surface->scheme();
    io.numberofregions = 1;
    io.regionlist = new double[io.numberofregions * 4];
    auto [x, y] = opts.region_point.value();
    io.regionlist[0] = x;
    io.regionlist[1] = y;
    io.regionlist[2] = surface->marker();
    io.regionlist[3] = opts.max_area.value();
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

Point
get_point(double * pts, int idx)
{
    double x = pts[idx * 2 + 0];
    double y = pts[idx * 2 + 1];
    return Point(x, y);
}

} // namespace tri

#endif

void
SchemeTriangle::mesh_surface(Ptr<MeshSurface> surface)
{
#ifdef KRADO_WITH_TRIANGLE
    Log::info("Meshing surface {}: scheme='triangle'", surface->id());

    auto has_region = this->opts_.region_point.has_value();
    auto has_max_area = this->opts_.max_area.has_value();

    triangulateio in, out;

    tri::init_io(in);
    tri::init_io(out);

    auto pt_id = tri::build_point_map(surface);
    tri::create_pslg(surface, pt_id, in);
    if (has_region)
        tri::create_regions(surface, this->opts_, in);

    // p = triangulate planar straight line graph
    // z = zero-based indexing
    // Q = quiet
    auto switches = fmt::format("pzQ");
    if (!has_region && has_max_area)
        switches += fmt::format("a{}", this->opts_.max_area.value());
    else
        switches += fmt::format("a");
    triangulate((char *) switches.c_str(), &in, &out, nullptr);

    SurfaceIndexMapper im(surface);
    for (int i = 0; i < out.numberoftriangles; i++) {
        std::array<Ptr<MeshVertexAbstract>, 3> tri;
        for (int j = 0; j < 3; j++) {
            auto vtx_id = out.trianglelist[i * 3 + j];
            auto pt = tri::get_point(out.pointlist, vtx_id);
            tri[j] = im.surface_vertex(pt.x, pt.y);
        }
        surface->add_triangle(tri);
    }

    tri::destroy_io(in);
    // region list is shallow copied into `out`
    if (in.regionlist)
        free(in.regionlist);
    tri::destroy_io(out);
#else
    throw Exception("krado was not built with triangle support.");
#endif
}

SchemeTriangle::SchemeTriangle(Options options) : Scheme2D("triangle"), opts_(options) {}

} // namespace krado
