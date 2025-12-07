#include "gmock/gmock.h"
#include "krado/polygon.h"
#include "krado/exception.h"

using namespace krado;

TEST(PolygonTest, ctor1)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0) };
    auto plgn = Polygon::create(pts);
}

TEST(PolygonTest, ctor_not_enough_points)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(1, 0, 0) };
    EXPECT_THROW({ Polygon::create(pts); }, Exception);
}
