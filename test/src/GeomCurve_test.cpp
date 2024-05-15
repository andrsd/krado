#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "builder.h"

using namespace krado;

const double PI = std::acos(-1);

TEST(GeomCurveTest, curve_type)
{
    GeomCurve line(testing::build_line(Point(0, 0, 0), Point(3, 4, 0)));
    EXPECT_EQ(line.type(), GeomCurve::Line);

    GeomCurve arc(testing::build_arc());
    EXPECT_EQ(arc.type(), GeomCurve::Circle);

    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);
    auto curves = gsurf.curves();
    EXPECT_EQ(curves[0].type(), GeomCurve::Circle);
}

TEST(GeomCurveTest, point)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve curve(line);

    auto pt_start = curve.point(0.);
    EXPECT_EQ(pt_start, Point(0., 0., 0.));

    auto pt_end = curve.point(5.);
    EXPECT_EQ(pt_end, Point(3., 4., 0.));

    auto pt_half = curve.point(2.5);
    EXPECT_EQ(pt_half, Point(1.5, 2., 0.));
}

TEST(GeomCurveTest, length)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve curve(line);
    EXPECT_DOUBLE_EQ(curve.length(), 5.);
}

TEST(GeomCurveTest, param_range)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve curve(line);
    auto [low, hi] = curve.param_range();
    EXPECT_DOUBLE_EQ(low, 0.);
    EXPECT_DOUBLE_EQ(hi, 5.);
}

TEST(GeomCurveTest, is_degenerated)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve curve(line);
    EXPECT_FALSE(curve.is_degenerated());
}

TEST(GeomCurveTest, curvature_line)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve curve(line);
    EXPECT_DOUBLE_EQ(curve.curvature(0.5), 0.);
}

TEST(GeomCurveTest, curvature_arc)
{
    auto arc = testing::build_arc();
    GeomCurve curve(arc);
    auto [lo, hi] = curve.param_range();
    EXPECT_DOUBLE_EQ(curve.curvature(lo), 1.);
    EXPECT_DOUBLE_EQ(curve.curvature((lo + hi) / 2.), 1.);
    EXPECT_DOUBLE_EQ(curve.curvature(hi), 1.);
}

TEST(GeomCurveTest, d1_line)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve curve(line);

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
    auto arc = testing::build_arc();
    GeomCurve curve(arc);
    EXPECT_EQ(curve.type(), GeomCurve::Circle);

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
    auto line = testing::build_line(Point(1, 2, 3), Point(3, 4, 5));
    GeomCurve curve(line);

    auto first = curve.first_vertex();
    EXPECT_DOUBLE_EQ(first.x(), 1.);
    EXPECT_DOUBLE_EQ(first.y(), 2.);
    EXPECT_DOUBLE_EQ(first.z(), 3.);

    auto last = curve.last_vertex();
    EXPECT_DOUBLE_EQ(last.x(), 3.);
    EXPECT_DOUBLE_EQ(last.y(), 4.);
    EXPECT_DOUBLE_EQ(last.z(), 5.);
}

TEST(GeomCurveTest, param_from_pt)
{
    auto line = testing::build_line(Point(1, 2, 3), Point(3, 4, 5));
    GeomCurve curve(line);

    auto [ulo, uhi] = curve.param_range();
    auto umid = (ulo + uhi) * 0.5;
    auto u = curve.parameter_from_point(Point(2, 3, 4));
    EXPECT_DOUBLE_EQ(u, umid);

    // point "outside" the curve
    EXPECT_THROW(curve.parameter_from_point(Point(5, 6, 7)), Exception);
}

TEST(GeomCurveTest, nearest_point)
{
    auto line = testing::build_line(Point(1, 2, 3), Point(3, 4, 5));
    GeomCurve curve(line);

    auto npt = curve.nearest_point(Point(1.2, 2.3, 3.1));
    EXPECT_EQ(npt, Point(1.2, 2.2, 3.2));

    EXPECT_THROW(curve.nearest_point(Point(5, 6, 7)), Exception);
}

TEST(GeomCurveTest, contains_point)
{
    auto line = testing::build_line(Point(1, 2, 3), Point(3, 4, 5));
    GeomCurve curve(line);

    EXPECT_TRUE(curve.contains_point(Point(1, 2, 3)));
    EXPECT_TRUE(curve.contains_point(Point(1.1, 2.1, 3.1)));

    EXPECT_FALSE(curve.contains_point(Point(1.2, 2.2, 3.1)));
}

TEST(GeomCurveTest, circle)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);
    auto curves = gsurf.curves();
    ASSERT_EQ(curves.size(), 1);
    auto [lo, hi] = curves[0].param_range();
    EXPECT_DOUBLE_EQ(lo, 0.);
    EXPECT_DOUBLE_EQ(hi, 2. * PI);
}
