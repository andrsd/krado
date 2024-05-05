#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "gp_Circ.hxx"
#include "BRepLib_MakeEdge.hxx"
#include "BRepLib_MakeWire.hxx"
#include "BRepLib_MakeFace.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Face.hxx"
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"

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

TEST(GeomSurfaceTest, point)
{
    auto circ_face = build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    auto pt_center = circ.point(0., 0.);
    EXPECT_NEAR(pt_center.x, 0., 1e-15);
    EXPECT_NEAR(pt_center.y, 0., 1e-15);
    EXPECT_NEAR(pt_center.z, 0., 1e-15);
}

TEST(GeomSurfaceTest, surface_area)
{
    auto circ_face = build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    EXPECT_DOUBLE_EQ(circ.area(), 4. * M_PI);
}

TEST(GeomSurfaceTest, d1_circ)
{
    auto circ_face = build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    auto [d1u, d1v] = circ.d1(0., 0.);

    EXPECT_DOUBLE_EQ(d1u.x, 1.);
    EXPECT_DOUBLE_EQ(d1u.y, 0.);
    EXPECT_DOUBLE_EQ(d1u.z, 0.);

    EXPECT_DOUBLE_EQ(d1v.x, 0.);
    EXPECT_DOUBLE_EQ(d1v.y, 1.);
    EXPECT_DOUBLE_EQ(d1v.z, 0.);
}

TEST(GeomSurfaceTest, op_topods_face)
{
    auto circ_face = build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    GProp_GProps props;
    BRepGProp::SurfaceProperties(circ, props);
    EXPECT_DOUBLE_EQ(props.Mass(), 4. * M_PI);
}
