#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/exception.h"

using namespace krado;

TEST(PointTest, ctor_empty)
{
    Point pt;
    EXPECT_DOUBLE_EQ(pt.x, 0.);
    EXPECT_DOUBLE_EQ(pt.y, 0.);
    EXPECT_DOUBLE_EQ(pt.z, 0.);
}

TEST(PointTest, ctor)
{
    Point pt(1., 2., 3.);
    EXPECT_DOUBLE_EQ(pt.x, 1.);
    EXPECT_DOUBLE_EQ(pt.y, 2.);
    EXPECT_DOUBLE_EQ(pt.z, 3.);
}

TEST(PointTest, oper_call)
{
    Point pt(1., 2., 3.);
    EXPECT_DOUBLE_EQ(pt(0), 1.);
    EXPECT_DOUBLE_EQ(pt(1), 2.);
    EXPECT_DOUBLE_EQ(pt(2), 3.);

    EXPECT_THROW({ pt(3); }, Exception);
}

TEST(PointTest, oper_plus)
{
    Point a(1., 2., 3.);
    Point b(2., -1., 4.);
    auto c = a + b;
    EXPECT_DOUBLE_EQ(c.x, 3.);
    EXPECT_DOUBLE_EQ(c.y, 1.);
    EXPECT_DOUBLE_EQ(c.z, 7.);
}

TEST(PointTest, oper_minus)
{
    Point a(1., 2., 3.);
    Point b(2., -1., 4.);
    auto c = a - b;
    EXPECT_DOUBLE_EQ(c.x, -1.);
    EXPECT_DOUBLE_EQ(c.y, 3.);
    EXPECT_DOUBLE_EQ(c.z, -1.);
}

TEST(PointTest, oper_add)
{
    Point a(1., 2., 3.);
    Point b(2., -1., 4.);
    a += b;
    EXPECT_DOUBLE_EQ(a.x, 3.);
    EXPECT_DOUBLE_EQ(a.y, 1.);
    EXPECT_DOUBLE_EQ(a.z, 7.);
}

TEST(PointTest, oper_sub)
{
    Point a(1., 2., 3.);
    Point b(2., -1., 4.);
    a -= b;
    EXPECT_DOUBLE_EQ(a.x, -1.);
    EXPECT_DOUBLE_EQ(a.y, 3.);
    EXPECT_DOUBLE_EQ(a.z, -1.);
}

TEST(PointTest, oper_mult_scalar)
{
    Point a(1., 2., 3.);
    a *= 3.;
    EXPECT_DOUBLE_EQ(a.x, 3.);
    EXPECT_DOUBLE_EQ(a.y, 6.);
    EXPECT_DOUBLE_EQ(a.z, 9.);
}

TEST(PointTest, oper_mult_scalar_2)
{
    Point a(1., 2., 3.);
    auto b = a * 2.;
    EXPECT_DOUBLE_EQ(b.x, 2.);
    EXPECT_DOUBLE_EQ(b.y, 4.);
    EXPECT_DOUBLE_EQ(b.z, 6.);
}

TEST(PointTest, oper_div_scalar)
{
    Point a(1., 2., 3.);
    a /= 2.;
    EXPECT_DOUBLE_EQ(a.x, 0.5);
    EXPECT_DOUBLE_EQ(a.y, 1.);
    EXPECT_DOUBLE_EQ(a.z, 1.5);
}

TEST(PointTest, distance)
{
    Point a(1., -2., 3.);
    Point b(2., -1., 6.);
    auto d = a.distance(b);
    EXPECT_DOUBLE_EQ(d, std::sqrt(11));
}
