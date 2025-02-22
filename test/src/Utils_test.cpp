#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/utils.h"

using namespace krado;

TEST(UtilsTest, angle)
{
    Point p1(0, 0, 0);
    Point p2(1, 0, 0);
    Point p3(1, 1, 0);
    auto alpha = utils::angle(p1, p2, p3);
    EXPECT_DOUBLE_EQ(alpha, M_PI / 2.);
}

TEST(UtilsTest, triangle_area)
{
    Point p1(0, 0, 0);
    Point p2(1, 0, 0);
    Point p3(1, 1, 0);
    auto area = utils::triangle_area(p1, p2, p3);
    EXPECT_DOUBLE_EQ(area, 0.5);
}
