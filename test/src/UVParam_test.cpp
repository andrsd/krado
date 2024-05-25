#include "gmock/gmock.h"
#include "krado/uv_param.h"

using namespace krado;

TEST(UVParamTest, default_constructor)
{
    UVParam param;
    EXPECT_EQ(param.u, 0.0);
    EXPECT_EQ(param.v, 0.0);
}

TEST(UVParamTest, parameterized_constructor)
{
    UVParam param(3.0, 4.0);
    EXPECT_EQ(param.u, 3.0);
    EXPECT_EQ(param.v, 4.0);
}

TEST(UVParamTest, operator_plus_equal)
{
    UVParam param1(3.0, 4.0);
    UVParam param2(1.0, 2.0);
    param1 += param2;
    EXPECT_EQ(param1.u, 4.0);
    EXPECT_EQ(param1.v, 6.0);
}

TEST(UVParamTest, operator_divide_equal)
{
    UVParam param1(3.0, 4.0);
    param1 /= 2.0;
    EXPECT_EQ(param1.u, 1.5);
    EXPECT_EQ(param1.v, 2.0);
}

TEST(UVParamTest, operator_plus)
{
    UVParam param1(3.0, 4.0);
    UVParam param2(1.0, 2.0);
    auto result = param1 + param2;
    EXPECT_EQ(result.u, 4.0);
    EXPECT_EQ(result.v, 6.0);
}

TEST(UVParamTest, operator_minus)
{
    UVParam param1(3.0, 5.0);
    UVParam param2(1.0, 2.0);
    auto result = param1 - param2;
    EXPECT_EQ(result.u, 2.0);
    EXPECT_EQ(result.v, 3.0);
}

TEST(UVParamTest, operator_multiply_scalar)
{
    UVParam param(3.0, 4.0);
    auto result = param * 2.0;
    EXPECT_EQ(result.u, 6.0);
    EXPECT_EQ(result.v, 8.0);
}

TEST(UVParamTest, unary_minus_operator)
{
    UVParam param(3.0, 4.0);
    auto result = -param;
    EXPECT_EQ(result.u, -3.0);
    EXPECT_EQ(result.v, -4.0);
}

TEST(UVParamTest, non_member_operator_multiply_scalar)
{
    UVParam param(3.0, 4.0);
    auto result = 2.0 * param;
    EXPECT_EQ(result.u, 6.0);
    EXPECT_EQ(result.v, 8.0);
}
