#include "krado/scheme_triangle.h"
#include "krado/mesh.h"
#include "krado/mesh_curve.h"
#include <map>

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
        n_segments += (int) c->curve_segments().size();
    io.numberofsegments = n_segments;
    io.segmentlist = (int *) malloc(n_segments * 2 * sizeof(int));

    int k = 0;
    for (auto & c : curves) {
        auto verts = c->all_vertices();
        auto & segments = c->curve_segments();
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
    auto & pars = surface.meshing_parameters();

    io.numberofregions = 1;
    io.regionlist = new double[io.numberofregions * 4];
    auto [x, y] = pars.get<std::tuple<double, double>>("region_point");
    io.regionlist[0] = x;
    io.regionlist[1] = y;
    io.regionlist[2] = pars.get<int>("marker");
    io.regionlist[3] = pars.get<double>("max_area");
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

} // namespace tri

SchemeTriangle::SchemeTriangle(Mesh & mesh, const Parameters & params) : Scheme(mesh, params)
{
    if (params.has<std::vector<Point>>("holes"))
        this->holes = params.get<std::vector<Point>>("holes");
}

void
SchemeTriangle::mesh_surface(MeshSurface & surface)
{
    triangulateio in, out;

    tri::init_io(in);
    tri::init_io(out);

    auto pt_id = tri::build_point_map(surface);
    tri::create_pslg(in, surface, pt_id);
    tri::create_regions(in, surface);

    // p = triangulate planar straight line graph
    // q = quality mesh
    // z = zero-based indexing
    // A = output area markers
    // Q = quiet
    triangulate((char *) "pqzAQ", &in, &out, nullptr);

    // map triangulation back onto our surface

    for (int i = 0, k = 0; i < out.numberoftriangles; i++) {
        for (int j = 0; j < 3; j++, k++)
            std::cerr << " " << out.trianglelist[k];
        std::cerr << std::endl;
    }

    tri::destroy_io(in);
    // region list is shallow copied into `out`
    if (in.regionlist)
        free(in.regionlist);
    tri::destroy_io(out);
}

#else

SchemeTriangle::SchemeTriangle(Mesh & mesh, const Parameters & params) : Scheme(mesh, params)
{
    throw Exception("krado was not built with triangle support.");
}

void
SchemeTriangle::mesh_surface(MeshSurface & msurface)
{
}

#endif

} // namespace krado
