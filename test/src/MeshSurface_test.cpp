#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme_equal.h"
#include "gp_Circ.hxx"
#include "BRepLib_MakeEdge.hxx"
#include "BRepLib_MakeWire.hxx"
#include "BRepLib_MakeFace.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Face.hxx"

using namespace krado;

namespace {

TopoDS_Face
build_circle(const Point & center, double radius)
{
    gp_Ax2 ax2;
    gp_Circ circ(ax2, radius);
    BRepLib_MakeEdge make_edge(circ);
    make_edge.Build();
    auto edge = make_edge.Edge();
    BRepLib_MakeWire make_wire(edge);
    make_wire.Build();
    auto wire = make_wire.Wire();
    BRepLib_MakeFace make_face(wire);
    make_face.Build();
    return make_face.Face();
}

} // namespace

TEST(MeshSurfaceTest, api)
{
    auto circ = build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);
    auto crvs = gsurf.curves();
    ASSERT_EQ(crvs.size(), 1);
    MeshCurve mcurve(crvs[0], nullptr, nullptr);

    MeshSurface msurface(gsurf, { &mcurve });
    EXPECT_EQ(&msurface.geom_surface(), &gsurf);

    auto & mpars = msurface.meshing_parameters();
    EXPECT_EQ(mpars.get<std::string>("scheme"), "auto");
    EXPECT_EQ(mpars.get<int>("marker"), 0);

    auto mcs = msurface.curves();
    EXPECT_EQ(mcs.size(), 1);
    EXPECT_EQ(mcs[0], &mcurve);

    auto mvtx0 = new MeshSurfaceVertex(gsurf, 0., 0.);
    msurface.add_vertex(mvtx0);
    auto mvtx1 = new MeshSurfaceVertex(gsurf, 0.1, 0.);
    msurface.add_vertex(mvtx1);
    auto mvtx2 = new MeshSurfaceVertex(gsurf, 0., 0.1);
    msurface.add_vertex(mvtx2);

    msurface.add_triangle(2, 0, 1);
    auto & triangles = msurface.triangles();
    ASSERT_EQ(triangles.size(), 1);
    auto & tri0 = triangles[0];
    EXPECT_EQ(tri0(0), 2);
    EXPECT_EQ(tri0(1), 0);
    EXPECT_EQ(tri0(2), 1);
}
