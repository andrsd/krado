#include <gmock/gmock.h>
#include "builder.h"
#include "krado/scheme/integral.h"
#include "krado/geom_curve.h"
#include "krado/vector.h"

using namespace krado;

TEST(IntegralTest, line)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));

    auto lc = [](double t) {
        // this matches the line parameter
        const double lo = 0.;
        const double hi = 5.;
        const double lc_lo = 0.1;
        const double lc_hi = 0.5;
        return lc_lo + (t - lo) * (lc_hi - lc_lo) / (hi - lo);
    };

    Integral igrl;
    igrl.integrate(line, [=](double t) {
        auto der = line.d1(t);
        return der.magnitude() / lc(t);
    });

    ASSERT_EQ(igrl.num_points(), 2467);
    auto pt0 = igrl.point(0);
    EXPECT_NEAR(pt0.t, 0., 1e-15);
    EXPECT_NEAR(pt0.lc, 10., 1e-15);
    EXPECT_NEAR(pt0.p, 0., 1e-15);

    auto pt1 = igrl.point(2466);
    EXPECT_NEAR(pt1.t, 5., 1e-15);
    EXPECT_NEAR(pt1.lc, 2., 1e-15);
    EXPECT_NEAR(pt1.p, 20.118, 1e-3);
}
