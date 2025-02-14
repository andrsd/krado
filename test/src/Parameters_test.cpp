#include "gtest/gtest.h"
#include "krado/parameters.h"
#include "ExceptionTestMacros.h"

using namespace krado;

TEST(ParametersTest, get)
{
    Parameters params;
    EXPECT_THROW_MSG(auto i = params.get<int>("i"), "No parameter 'i' found.");
}

TEST(ParametersTest, param_value)
{
    Parameters params;
    params.add_param<double>("param", 12.34);
    EXPECT_EQ(params.get<double>("param"), 12.34);
}

TEST(ParametersTest, has_value)
{
    Parameters params;
    params.add_param<double>("param", 12.34);
    EXPECT_EQ(params.has<double>("param"), true);
    params.clear();
    EXPECT_EQ(params.has<double>("param"), false);
}

TEST(ParametersTest, assign)
{
    Parameters params1;
    params1.add_param<double>("param", 12.34);

    Parameters params2 = params1;
    EXPECT_EQ(params2.has<double>("param"), true);
    EXPECT_EQ(params2.get<double>("param"), 12.34);
}

TEST(ParametersTest, add_params)
{
    Parameters params1;
    params1.add_param<double>("p1", 12.34);

    Parameters params2;
    params1.add_param<double>("p2");

    params1 += params2;
    EXPECT_EQ(params1.has<double>("p1"), true);
    EXPECT_EQ(params1.get<double>("p1"), 12.34);
    EXPECT_EQ(params1.has<double>("p2"), true);
}

Parameters
validParams1()
{
    Parameters params;
    params.add_param<double>("p", 78.56);
    return params;
}

TEST(ParametersTest, valid_params)
{
    Parameters params1 = validParams1();
    EXPECT_EQ(params1.get<double>("p"), 78.56);
    EXPECT_TRUE(params1.is_param_valid("p"));
}
