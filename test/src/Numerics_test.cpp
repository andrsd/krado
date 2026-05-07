#include "gmock/gmock.h"
#include "krado/numerics.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/uv_param.h"

using namespace krado;

TEST(NumericsTest, sys_2x2)
{
    std::array<std::array<double, 2>, 2> A;
    A[0] = { 1., 0. };
    A[1] = { 0., 1. };
    std::array<double, 2> b = { 3, 5 };
    auto res = sys2x2(A, b);
    ASSERT_TRUE(res.has_value());
    auto x = res.value();
    EXPECT_THAT(x[0], 3.);
    EXPECT_THAT(x[1], 5.);
}

TEST(NumericsTest, normal_xyz)
{
    Point a(0, 0, 0);
    Point b(1, 1, 0);
    Point c(1, 0, 0);
    auto n = normal(a, b, c);
    EXPECT_NEAR(n.x, 0., 1e-15);
    EXPECT_NEAR(n.y, 0., 1e-15);
    EXPECT_NEAR(n.z, 1., 1e-15);
}

TEST(NumericsTest, normal_uv)
{
    UVParam a(0, 0);
    UVParam b(1, 0);
    UVParam c(1, 1);
    auto n = normal(a, b, c);
    EXPECT_NEAR(n.x, 0., 1e-15);
    EXPECT_NEAR(n.y, 0., 1e-15);
    EXPECT_NEAR(n.z, -1., 1e-15);
}
