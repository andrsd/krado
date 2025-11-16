#include "gmock/gmock.h"
#include "krado/ops.h"
#include "krado/point.h"
#include "krado/box.h"

using namespace krado;

TEST(OperationsTest, fuse)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = fuse(box1, box2);
}

TEST(OperationsTest, fuse_multiple)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    Box box3(Point(1, 1, 0), Point(2, 2, -1));
    auto res = fuse({ box1, box2, box3 });
}

TEST(OperationsTest, cut)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = cut(box1, box2);
}

TEST(OperationsTest, intersect)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = intersect(box1, box2);
}
