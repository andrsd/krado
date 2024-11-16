#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/transform.h"

using namespace krado;

TEST(TransformTest, idetity)
{
    auto t = Trsf::identity();
    Point pt(2, 3, 4);
    auto pt2 = t * pt;
    EXPECT_NEAR(pt2.x, 2, 1e-10);
    EXPECT_NEAR(pt2.y, 3, 1e-10);
    EXPECT_NEAR(pt2.z, 4, 1e-10);
}

TEST(TransformTest, scale_isotropic)
{
    Point pt(2, 3, 4);
    auto pt2 = Trsf::scale(2) * pt;
    EXPECT_NEAR(pt2.x, 4, 1e-10);
    EXPECT_NEAR(pt2.y, 6, 1e-10);
    EXPECT_NEAR(pt2.z, 8, 1e-10);
}

TEST(TransformTest, scale_unisotropic)
{
    Point pt(2, 3, 4);
    auto pt2 = Trsf::scale(3, 5, 9) * pt;
    EXPECT_NEAR(pt2.x, 6, 1e-10);
    EXPECT_NEAR(pt2.y, 15, 1e-10);
    EXPECT_NEAR(pt2.z, 36, 1e-10);
}

TEST(TransformTest, translate)
{
    Point pt(2, 3, 4);
    auto pt2 = Trsf::translate(3, 5, 9) * pt;
    EXPECT_NEAR(pt2.x, 5, 1e-10);
    EXPECT_NEAR(pt2.y, 8, 1e-10);
    EXPECT_NEAR(pt2.z, 13, 1e-10);
}

TEST(TransformTest, rotate_x)
{
    Point pt(0, 2, 0);
    auto pt2 = Trsf::rotate_x(M_PI / 2.) * pt;
    EXPECT_NEAR(pt2.x, 0, 1e-10);
    EXPECT_NEAR(pt2.y, 0, 1e-10);
    EXPECT_NEAR(pt2.z, 2, 1e-10);
}

TEST(TransformTest, rotate_y)
{
    Point pt(2, 0, 0);
    auto pt2 = Trsf::rotate_y(M_PI / 2.) * pt;
    EXPECT_NEAR(pt2.x, 0, 1e-10);
    EXPECT_NEAR(pt2.y, 0, 1e-10);
    EXPECT_NEAR(pt2.z, -2, 1e-10);
}

TEST(TransformTest, rotate_z)
{
    Point pt(2, 0, 0);
    auto pt2 = Trsf::rotate_z(M_PI / 2.) * pt;
    EXPECT_NEAR(pt2.x, 0, 1e-10);
    EXPECT_NEAR(pt2.y, 2, 1e-10);
    EXPECT_NEAR(pt2.z, 0, 1e-10);
}

TEST(TransformTest, chain_ops)
{
    Point pt(2, 3, 4);
    auto pt2 = Trsf::scale(2) * Trsf::translate(3, 5, 9) * pt;
    EXPECT_NEAR(pt2.x, 7, 1e-10);
    EXPECT_NEAR(pt2.y, 11, 1e-10);
    EXPECT_NEAR(pt2.z, 17, 1e-10);
}
