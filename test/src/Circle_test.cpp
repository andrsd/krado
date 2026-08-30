#include <gmock/gmock.h>
#include "builder.h"
#include "krado/circle.h"

using namespace krado;

TEST(CircleTest, create_1)
{
    auto circ = Circle::create(Point(1, 2, 3), 0.5);

    EXPECT_NEAR(circ.radius(), 0.5, 1e-6);
    EXPECT_NEAR(circ.area(), 0.785398, 1e-6);
    EXPECT_NEAR(circ.length(), 3.141592, 1e-6);

    auto loc = circ.location();
    EXPECT_NEAR(loc.x, 1, 1e-15);
    EXPECT_NEAR(loc.y, 2, 1e-15);
    EXPECT_NEAR(loc.z, 3, 1e-15);
}

TEST(CircleTest, create_2)
{
    auto circ = Circle::create(Point(1, 2, 3), 0.5, Vector(0, 0, 1));

    EXPECT_NEAR(circ.radius(), 0.5, 1e-6);
    EXPECT_NEAR(circ.area(), 0.785398, 1e-6);
    EXPECT_NEAR(circ.length(), 3.141592, 1e-6);

    auto loc = circ.location();
    EXPECT_NEAR(loc.x, 1, 1e-15);
    EXPECT_NEAR(loc.y, 2, 1e-15);
    EXPECT_NEAR(loc.z, 3, 1e-15);
}

TEST(CircleTest, create_3)
{
    auto circ = Circle::create(Point(1, 2, 3), Point(1.5, 2, 3), Vector(1, 0, 0));

    EXPECT_NEAR(circ.radius(), 0.5, 1e-6);
    EXPECT_NEAR(circ.area(), 0.785398, 1e-6);
    EXPECT_NEAR(circ.length(), 3.141592, 1e-6);

    auto loc = circ.location();
    EXPECT_NEAR(loc.x, 1, 1e-15);
    EXPECT_NEAR(loc.y, 2, 1e-15);
    EXPECT_NEAR(loc.z, 3, 1e-15);
}

TEST(CircleTest, create_4)
{
    auto circ = Circle::create(Point(0.5, 2, 3), Point(1.5, 2, 3), Point(1, 2.5, 3));

    EXPECT_NEAR(circ.radius(), 0.5, 1e-6);
    EXPECT_NEAR(circ.area(), 0.785398, 1e-6);
    EXPECT_NEAR(circ.length(), 3.141592, 1e-6);

    auto loc = circ.location();
    EXPECT_NEAR(loc.x, 1, 1e-15);
    EXPECT_NEAR(loc.y, 2, 1e-15);
    EXPECT_NEAR(loc.z, 3, 1e-15);
}
