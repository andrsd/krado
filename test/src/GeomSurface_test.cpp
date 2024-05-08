#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "gp_Circ.hxx"
#include "BRepLib_MakeEdge.hxx"
#include "BRepLib_MakeWire.hxx"
#include "BRepLib_MakeFace.hxx"
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

TopoDS_Face
build_triangle(const Point & center, double radius)
{
    gp_Pnt ctr(center.x, center.y, center.z);
    gp_Pnt pt1(center.x, center.y + radius, center.z);
    gp_Pnt pt2(center.x + radius, center.y, center.z);

    BRepLib_MakeEdge make_edge0(pt1, pt2);
    make_edge0.Build();
    auto edge0 = make_edge0.Edge();

    BRepLib_MakeEdge make_edge1(ctr, pt1);
    make_edge1.Build();
    auto edge1 = make_edge1.Edge();

    BRepLib_MakeEdge make_edge2(ctr, pt2);
    make_edge2.Build();
    auto edge2 = make_edge2.Edge();

    BRepLib_MakeWire make_wire(edge0, edge1, edge2);
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

TEST(GeomSurfaceTest, param_range)
{
    auto circ_face = build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    auto [u_lo, u_hi] = circ.param_range(0);
    EXPECT_DOUBLE_EQ(u_lo, -2);
    EXPECT_DOUBLE_EQ(u_hi, 2.);

    auto [v_lo, v_hi] = circ.param_range(1);
    EXPECT_DOUBLE_EQ(v_lo, -2.);
    EXPECT_DOUBLE_EQ(v_hi, 2.);

    EXPECT_THROW({ circ.param_range(2); }, Exception);
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

TEST(GeomSurfaceTest, curves)
{
    constexpr double r = 2.;
    auto qcirc_face = build_triangle(Point(0, 0, 0), r);
    GeomSurface circ(qcirc_face);

    auto crvs = circ.curves();
    EXPECT_EQ(crvs.size(), 3);
    EXPECT_DOUBLE_EQ(crvs[0].length(), std::sqrt(8));
    EXPECT_DOUBLE_EQ(crvs[1].length(), r);
    EXPECT_DOUBLE_EQ(crvs[2].length(), r);
}

TEST(GeomSurfaceTest, normal)
{
    auto circ_face = build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    auto n = circ.normal(0., 0.);
    EXPECT_DOUBLE_EQ(n.x, 0.);
    EXPECT_DOUBLE_EQ(n.y, 0.);
    EXPECT_DOUBLE_EQ(n.z, 1.);
}

TEST(GeomSurfaceTest, param_from_pt)
{
    constexpr double r = 2.;
    auto qcirc_face = build_triangle(Point(0, 0, 0), r);
    GeomSurface circ(qcirc_face);

    auto [u, v] = circ.parameter_from_point(Point(0.5, 1, 0));
    EXPECT_DOUBLE_EQ(u, 0.2071067811865474);
    EXPECT_DOUBLE_EQ(v, 0.2928932188134525);

    // point "outside" the surface
    EXPECT_THROW(circ.parameter_from_point(Point(3, 3, 0)), Exception);
}
