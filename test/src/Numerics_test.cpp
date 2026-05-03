#include "gmock/gmock.h"
#include "krado/numerics.h"
#include "krado/uv_param.h"
#include "krado/point.h"

using namespace krado;

TEST(NumericsTest, circum_center_uv)
{
    UVParam pa(0, 0);
    UVParam pb(1, 0);
    UVParam pc(1, 1);
    auto center = circum_center(pa, pb, pc);
    ASSERT_TRUE(center.has_value());
    auto ctr = center.value();
    EXPECT_NEAR(ctr.u, 0.5, 1e-10);
    EXPECT_NEAR(ctr.v, 0.5, 1e-10);
}

TEST(NumericsTest, circum_center_xyz)
{
    Point pa(0, 0, 0);
    Point pb(1, 0, 0);
    Point pc(1, 1, 0);
    auto center = circum_center(pa, pb, pc);
    EXPECT_NEAR(center.x, 0.5, 1e-10);
    EXPECT_NEAR(center.y, 0.5, 1e-10);
    EXPECT_NEAR(center.z, 0., 1e-10);
}
