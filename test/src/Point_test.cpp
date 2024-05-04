#include "gmock/gmock.h"
#include "krado/point.h"

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
