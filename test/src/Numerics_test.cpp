#include "gmock/gmock.h"
#include "krado/numerics.h"

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
