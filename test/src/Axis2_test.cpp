#include <gmock/gmock.h>
#include "krado/axis2.h"
#include "krado/exception.h"

using namespace krado;

TEST(Axis2Test, ctor)
{
    Axis2 ax;
    EXPECT_EQ(ax.location(), Point(0, 0, 0));
    EXPECT_EQ(ax.x_direction(), Vector(1, 0, 0));
    EXPECT_EQ(ax.y_direction(), Vector(0, 1, 0));
    EXPECT_EQ(ax.direction(), Vector(0, 0, 1));
}

TEST(Axis2Test, ctor_dir)
{
    Point origin(1, 2, 3);
    Vector normal(0, 0, 1);

    Axis2 ax(origin, normal);

    EXPECT_EQ(ax.location(), origin);
    EXPECT_NEAR(dot_product(ax.direction(), normal.normalized()), 1.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.x_direction(), ax.direction()), 0.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.y_direction(), ax.direction()), 0.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.x_direction(), ax.y_direction()), 0.0, 1e-12);
}

TEST(Axis2Test, ctor_n_dir)
{
    Point origin(0, 0, 0);
    Vector normal(0, 0, 1);
    Vector vx(1, 0, 0);

    Axis2 ax(origin, normal, vx);

    EXPECT_EQ(ax.location(), origin);
    EXPECT_NEAR(dot_product(ax.direction(), normal.normalized()), 1.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.x_direction(), ax.direction()), 0.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.y_direction(), ax.direction()), 0.0, 1e-12);
    EXPECT_NEAR(dot_product(ax.x_direction(), ax.y_direction()), 0.0, 1e-12);
}

TEST(Axis2Test, local2world)
{
    Point origin(10, 0, 0);
    Vector normal(0, 0, 1);
    Vector vx_hint(1, 0, 0);

    Axis2 ax(origin, normal, vx_hint);

    auto p1 = ax.local_to_world(0, 0);
    auto p2 = ax.local_to_world(1, 0);
    auto p3 = ax.local_to_world(0, 2);

    EXPECT_EQ(p1, origin);
    EXPECT_EQ(p2, Point(11, 0, 0));
    EXPECT_EQ(p3, Point(10, 2, 0));
}

TEST(Axis2Test, parallel_vecs)
{
    Point origin(0, 0, 0);
    Vector N(0, 0, 1);
    Vector Vx(0, 0, -2); // opposite direction

    EXPECT_THROW({ Axis2 ax(origin, N, Vx); }, Exception);
}
