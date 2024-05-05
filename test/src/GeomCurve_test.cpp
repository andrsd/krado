#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "Geom_Line.hxx"
#include "BRepLib_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "GC_MakeArcOfCircle.hxx"

using namespace krado;

TEST(GeomCurveTest, point)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());

    auto pt_start = curve.point(0.);
    EXPECT_DOUBLE_EQ(pt_start.x, 0.);
    EXPECT_DOUBLE_EQ(pt_start.y, 0.);
    EXPECT_DOUBLE_EQ(pt_start.z, 0.);

    auto pt_end = curve.point(5.);
    EXPECT_DOUBLE_EQ(pt_end.x, 3.);
    EXPECT_DOUBLE_EQ(pt_end.y, 4.);
    EXPECT_DOUBLE_EQ(pt_end.z, 0.);

    auto pt_half = curve.point(2.5);
    EXPECT_DOUBLE_EQ(pt_half.x, 1.5);
    EXPECT_DOUBLE_EQ(pt_half.y, 2.);
    EXPECT_DOUBLE_EQ(pt_half.z, 0.);
}

TEST(GeomCurveTest, length)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());
    EXPECT_DOUBLE_EQ(curve.length(), 5.);
}

TEST(GeomCurveTest, param_range)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());
    auto [low, hi] = curve.param_range();
    EXPECT_DOUBLE_EQ(low, 0.);
    EXPECT_DOUBLE_EQ(hi, 5.);
}

TEST(GeomCurveTest, is_degenerated)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());
    EXPECT_FALSE(curve.is_degenerated());
}

TEST(GeomCurveTest, curvature_line)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());
    EXPECT_DOUBLE_EQ(curve.curvature(0.5), 0.);
}

TEST(GeomCurveTest, curvature_arc)
{
    gp_Pnt pt1(-1, 0, 0);
    gp_Pnt pt2(0, 1, 0);
    gp_Pnt pt3(1, 0, 0);
    GC_MakeArcOfCircle mk_arc(pt1, pt2, pt3);
    BRepBuilderAPI_MakeEdge make_edge(mk_arc.Value());
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());
    auto [lo, hi] = curve.param_range();
    EXPECT_DOUBLE_EQ(curve.curvature(lo), 1.);
    EXPECT_DOUBLE_EQ(curve.curvature((lo + hi) / 2.), 1.);
    EXPECT_DOUBLE_EQ(curve.curvature(hi), 1.);
}

TEST(GeomCurveTest, d1_line)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());

    auto v_0 = curve.d1(0.);
    EXPECT_DOUBLE_EQ(v_0.x, 0.6);
    EXPECT_DOUBLE_EQ(v_0.y, 0.8);
    EXPECT_DOUBLE_EQ(v_0.z, 0.);

    auto v_1 = curve.d1(curve.length());
    EXPECT_DOUBLE_EQ(v_1.x, 0.6);
    EXPECT_DOUBLE_EQ(v_1.y, 0.8);
    EXPECT_DOUBLE_EQ(v_1.z, 0.);
}

TEST(GeomCurveTest, d1_arc)
{
    gp_Pnt pt1(-1, 0, 0);
    gp_Pnt pt2(0, 1, 0);
    gp_Pnt pt3(1, 0, 0);
    GC_MakeArcOfCircle mk_arc(pt1, pt2, pt3);
    BRepBuilderAPI_MakeEdge make_edge(mk_arc.Value());
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());
    auto [lo, hi] = curve.param_range();

    auto v_0 = curve.d1(lo);
    EXPECT_NEAR(v_0.x, 0., 1e-10);
    EXPECT_NEAR(v_0.y, 1., 1e-10);
    EXPECT_NEAR(v_0.z, 0., 1e-10);

    auto v_1 = curve.d1((lo + hi) / 2.);
    EXPECT_NEAR(v_1.x, 1., 1e-10);
    EXPECT_NEAR(v_1.y, 0., 1e-10);
    EXPECT_NEAR(v_1.z, 0., 1e-10);

    auto v_2 = curve.d1(hi);
    EXPECT_NEAR(v_2.x, 0., 1e-10);
    EXPECT_NEAR(v_2.y, -1., 1e-10);
    EXPECT_NEAR(v_2.z, 0., 1e-10);
}

TEST(GeomCurveTest, vertices)
{
    gp_Pnt pt1(1, 2, 3);
    gp_Pnt pt2(3, 4, 5);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::GeomCurve curve(make_edge.Edge());

    auto first = curve.first_vertex();
    EXPECT_DOUBLE_EQ(first.x(), 1.);
    EXPECT_DOUBLE_EQ(first.y(), 2.);
    EXPECT_DOUBLE_EQ(first.z(), 3.);

    auto last = curve.last_vertex();
    EXPECT_DOUBLE_EQ(last.x(), 3.);
    EXPECT_DOUBLE_EQ(last.y(), 4.);
    EXPECT_DOUBLE_EQ(last.z(), 5.);
}
