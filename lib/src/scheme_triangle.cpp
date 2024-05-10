// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme_triangle.h"
#include "krado/mesh.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include <map>
#include <array>
#include <set>

#ifdef KRADO_WITH_TRIANGLE
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
build_point_map(const MeshSurface & surface)
{
    std::map<Point, int> pt_id;
    auto & curves = surface.curves();
    for (auto & c : curves) {
        auto verts = c->all_vertices();
        for (auto & vtx : verts) {
            auto pt = vtx->point();
            auto id = pt_id.size();
            pt_id.try_emplace(pt, id);
        }
    }
    return pt_id;
}

void
create_point_list(triangulateio & io, const std::map<Point, int> & pt_id)
{
    io.numberofpoints = (int) pt_id.size();
    io.pointlist = (double *) malloc(io.numberofpoints * 2 * sizeof(double));
    size_t k = 0;
    for (auto & [pt, id] : pt_id) {
        io.pointlist[k++] = pt.x;
        io.pointlist[k++] = pt.y;
    }
}

void
create_segment_list(triangulateio & io,
                    const MeshSurface & surface,
                    const std::map<Point, int> & pt_id)
{
    // segments
    int n_segments = 0;
    auto & curves = surface.curves();
    for (auto & c : curves)
        n_segments += (int) c->segments().size();
    io.numberofsegments = n_segments;
    io.segmentlist = (int *) malloc(n_segments * 2 * sizeof(int));

    int k = 0;
    for (auto & c : curves) {
        auto verts = c->all_vertices();
        auto & segments = c->segments();
        for (auto & s : segments) {
            auto v1 = s.vertex_id(0);
            auto v2 = s.vertex_id(1);

            auto pt1 = verts[v1]->point();
            auto pt2 = verts[v2]->point();

            auto id1 = pt_id.at(pt1);
            auto id2 = pt_id.at(pt2);
            io.segmentlist[k++] = id1;
            io.segmentlist[k++] = id2;
        }
    }
}

void
create_pslg(triangulateio & io, const MeshSurface & surface, const std::map<Point, int> & pt_id)
{
    create_point_list(io, pt_id);
    create_segment_list(io, surface, pt_id);
}

void
create_regions(triangulateio & io, const MeshSurface & surface)
{
    auto & scheme = surface.scheme();
    io.numberofregions = 1;
    io.regionlist = new double[io.numberofregions * 4];
    auto [x, y] = scheme.get<std::tuple<double, double>>("region_point");
    io.regionlist[0] = x;
    io.regionlist[1] = y;
    io.regionlist[2] = surface.get<int>("marker");
    io.regionlist[3] = scheme.get<double>("max_area");
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

std::map<Point, int>
build_vertex_map(const MeshSurface & surface)
{
    std::map<Point, int> pt_vtx;
    auto verts = surface.all_vertices();
    for (int i = 0; i < verts.size(); i++) {
        auto & vtx = verts[i];
        auto pt = vtx->point();
        pt_vtx.try_emplace(pt, i);
    }
    return pt_vtx;
}

Point
create_point(double * pts, int idx)
{
    double x = pts[idx * 2 + 0];
    double y = pts[idx * 2 + 1];
    return Point(x, y);
}

std::array<int, 3>
map_triangle(double * coords, const std::map<Point, int> & vtx_map, int tri[3])
{
    Point pts[3];
    for (int i = 0; i < 3; i++)
        pts[i] = create_point(coords, tri[i]);

    try {
        std::array<int, 3> vtx_idx;
        for (int i = 0; i < 3; i++) {
            auto idx = vtx_map.at(pts[i]);
            vtx_idx[i] = idx;
        }
        return vtx_idx;
    }
    catch (...) {
        throw Exception("Failed to map a triangle back onto mesh surface.");
    }
}

void
copy_vertices_into_surface(MeshSurface & surface,
                           const triangulateio & io,
                           const std::map<Point, int> & pt_id)
{
    // Insert vertices from curves into the surface
    auto & curves = surface.curves();
    std::set<MeshVertexAbstract *> vtx_set;
    for (auto & c : curves) {
        for (auto & v : c->bounding_vertices()) {
            auto [it, added] = vtx_set.insert(v);
            if (added)
                surface.add_vertex(v);
        }
        for (auto & v : c->curve_vertices()) {
            auto [it, added] = vtx_set.insert(v);
            if (added)
                surface.add_vertex(v);
        }
    }

    // Create vertices for all new points. They will be MeshSurfaceVertices, because we used
    // constrained Delaunay triangulation, i.e. no edges were split, so we don't have new
    // MeshCurveVertices
    for (int i = 0; i < io.numberofpoints; i++) {
        auto pt = tri::create_point(io.pointlist, i);
        if (pt_id.count(pt) > 0) {
            // this is a known point, either vertex or a curve vertex
        }
        else {
            // this is a new point, create a new vertex on the surface
            auto gsurf = surface.geom_surface();
            auto [u, v] = gsurf.parameter_from_point(pt);
            auto * svtx = new MeshSurfaceVertex(gsurf, u, v);
            surface.add_vertex(svtx);
        }
    }
}

} // namespace tri

#endif

void
SchemeTriangle::mesh_surface(MeshSurface & surface)
{
#ifdef KRADO_WITH_TRIANGLE
    triangulateio in, out;

    tri::init_io(in);
    tri::init_io(out);

    auto pt_id = tri::build_point_map(surface);
    tri::create_pslg(in, surface, pt_id);

    // p = triangulate planar straight line graph
    // z = zero-based indexing
    // Q = quiet
    auto switches = fmt::format("pzQ");
    triangulate((char *) switches.c_str(), &in, &out, nullptr);

    tri::copy_vertices_into_surface(surface, out, pt_id);

    // map triangulation back onto our surface
    auto vtx_map = tri::build_vertex_map(surface);
    for (int i = 0; i < out.numberoftriangles; i++) {
        int tritri[3] = { out.trianglelist[i * 3 + 0],
                          out.trianglelist[i * 3 + 1],
                          out.trianglelist[i * 3 + 2] };
        auto tri3 = tri::map_triangle(out.pointlist, vtx_map, tritri);
        surface.add_triangle(tri3);
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

SchemeTriangle::SchemeTriangle() : Scheme("triangle")
{
}

} // namespace krado
