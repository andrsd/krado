#include <gmock/gmock.h>
#include "krado/axis1.h"

using namespace krado;

TEST(Axis1Test, ctor_pt_dir)
{
    Point origin(1, 2, 3);
    Vector dir(0, 0, 5);

    Axis1 ax(origin, dir);

    EXPECT_EQ(ax.location(), origin);
    EXPECT_NEAR(ax.direction().magnitude(), 1.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.direction(), Vector(0, 0, 1)), 1.0, 1e-12);
}

TEST(Axis1Test, is_equal_true)
{
    Point p1(1, 1, 1);
    Vector d1(1, 0, 0);

    Axis1 a1(p1, d1);
    Axis1 a2(p1, Vector(2, 0, 0)); // same direction, different magnitude

    EXPECT_TRUE(a1.is_equal(a2, 1e-8));
}

TEST(Axis1Test, is_equal_false_on_direction)
{
    Axis1 a1(Point(0, 0, 0), Vector(1, 0, 0));
    Axis1 a2(Point(0, 0, 0), Vector(0, 1, 0));

    EXPECT_FALSE(a1.is_equal(a2, 1e-8));
}

TEST(Axis1Test, is_equal_false_on_location)
{
    Axis1 a1(Point(0, 0, 0), Vector(1, 0, 0));
    Axis1 a2(Point(1e-3, 0, 0), Vector(1, 0, 0));

    EXPECT_FALSE(a1.is_equal(a2, 1e-6));
}
