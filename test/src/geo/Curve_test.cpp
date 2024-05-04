#include "gmock/gmock.h"
#include "krado/geo/curve.h"
#include "Geom_Line.hxx"
#include "BRepLib_MakeEdge.hxx"

using namespace krado;

TEST(Geo_CurveTest, point)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::Curve curve(make_edge.Edge());

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

TEST(Geo_CurveTest, length)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::Curve curve(make_edge.Edge());
    EXPECT_DOUBLE_EQ(curve.length(), 5.);
}

TEST(Geo_CurveTest, param_range)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::Curve curve(make_edge.Edge());
    auto [low, hi] = curve.param_range();
    EXPECT_DOUBLE_EQ(low, 0.);
    EXPECT_DOUBLE_EQ(hi, 5.);
}
