#include <gmock/gmock.h>
#include "krado/axis2.h"
#include "krado/exception.h"

using namespace krado;

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

TEST(Axis2Test, parallel_vecs)
{
    Point origin(0, 0, 0);
    Vector N(0, 0, 1);
    Vector Vx(0, 0, -2); // opposite direction

    EXPECT_THROW({ Axis2 ax(origin, N, Vx); }, Exception);
}
