#include "gmock/gmock.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/geom_surface.h"
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

TEST(MeshSurfaceVertexTest, test)
{
    auto circ = build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);

    auto [u, v] = gsurf.parameter_from_point(Point(0.5, 1., 0.));

    MeshSurfaceVertex msvtx(gsurf, u, v);

    EXPECT_EQ(&msvtx.geom_surface(), &gsurf);

    auto pt = msvtx.point();
    EXPECT_DOUBLE_EQ(pt.x, 0.5);
    EXPECT_DOUBLE_EQ(pt.y, 1.);
    EXPECT_DOUBLE_EQ(pt.z, 0.);

    auto [vtx_u, vtx_v] = msvtx.parameter();
    EXPECT_DOUBLE_EQ(vtx_u, u);
    EXPECT_DOUBLE_EQ(vtx_v, v);
}
