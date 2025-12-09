#include "gmock/gmock.h"
#include <cmath>
#include "krado/geom_curve.h"
#include "krado/scheme/integral.h"
#include "krado/point.h"
#include "builder.h"

using namespace krado;

TEST(SchemeIntegralTest, arc_length_of_a_line)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    Integral igrl;
    igrl.integrate(line, [=](double t) {
        auto der = line.d1(t);
        return der.magnitude();
    });

    EXPECT_EQ(igrl.num_point(), 257);

    auto lo = igrl.point(0);
    EXPECT_NEAR(lo.t, 0, 1e-15);
    EXPECT_NEAR(lo.p, 0, 1e-15);
    EXPECT_NEAR(lo.lc, 1, 1e-15);

    auto hi = igrl.point(256);
    EXPECT_NEAR(hi.t, 1, 1e-15);
    EXPECT_NEAR(hi.p, 1, 1e-15);
    EXPECT_NEAR(hi.lc, 1, 1e-15);
}

TEST(SchemeIntegralTest, arc_length_of_an_arc)
{
    auto arc = testing::build_arc();
    Integral igrl;
    igrl.integrate(arc, [=](double t) {
        auto der = arc.d1(t);
        return der.magnitude();
    });

    EXPECT_EQ(igrl.num_point(), 257);

    auto lo = igrl.point(0);
    EXPECT_NEAR(lo.t, 0, 1e-15);
    EXPECT_NEAR(lo.p, 0, 1e-15);
    EXPECT_NEAR(lo.lc, 1, 1e-15);

    auto hi = igrl.point(256);
    EXPECT_NEAR(hi.t, M_PI, 1e-15);
    EXPECT_NEAR(hi.p, M_PI, 1e-15);
    EXPECT_NEAR(hi.lc, 1., 1e-15);
}
