#include "gmock/gmock.h"
#include "krado/spline.h"

using namespace krado;

TEST(SplineTest, ctor1)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0) };
    auto spline = Spline::create(pts);
}

TEST(SplineTest, ctor2)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0) };
    Vector tg1(1, 0, 0);
    Vector tg2(0, 1, 0);
    auto spline = Spline::create(pts, tg1, tg2);
}
