#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/mesh_surface.h"
#include "krado/equal.h"
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

    EXPECT_EQ(msurface.marker(), 0);

    msurface.set_marker(123);
    EXPECT_EQ(msurface.marker(), 123);

    auto mcs = msurface.curves();
    EXPECT_EQ(mcs.size(), 1);
    EXPECT_EQ(mcs[0], &mcurve);
}
