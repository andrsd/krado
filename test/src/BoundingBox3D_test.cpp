#include "gmock/gmock.h"
#include "krado/bounding_box_3d.h"
#include "krado/exception.h"

using namespace krado;

constexpr double MAX = std::numeric_limits<double>::max();

TEST(BoundingBox3DTest, ctor_empty)
{
    BoundingBox3D bbox;

    auto min = bbox.min();
    EXPECT_DOUBLE_EQ(min.x, MAX);
    EXPECT_DOUBLE_EQ(min.y, MAX);
    EXPECT_DOUBLE_EQ(min.z, MAX);

    auto max = bbox.max();
    EXPECT_DOUBLE_EQ(max.x, -MAX);
    EXPECT_DOUBLE_EQ(max.y, -MAX);
    EXPECT_DOUBLE_EQ(max.z, -MAX);
}

TEST(BoundingBox3DTest, ctor_1)
{
    Point pt(1, 2, 3);
    BoundingBox3D bbox(pt);

    EXPECT_DOUBLE_EQ(bbox.min().distance(pt), 0);
    EXPECT_DOUBLE_EQ(bbox.max().distance(pt), 0);
}

TEST(BoundingBox3DTest, ctor_6)
{
    BoundingBox3D bbox(-1, -2, -3, 1, 2, 3);

    auto min = bbox.min();
    EXPECT_DOUBLE_EQ(min.x, -1);
    EXPECT_DOUBLE_EQ(min.y, -2);
    EXPECT_DOUBLE_EQ(min.z, -3);

    auto max = bbox.max();
    EXPECT_DOUBLE_EQ(max.x, 1);
    EXPECT_DOUBLE_EQ(max.y, 2);
    EXPECT_DOUBLE_EQ(max.z, 3);
}

TEST(BoundingBox3DTest, empty)
{
    BoundingBox3D bbox;
    EXPECT_TRUE(bbox.empty());

    bbox += Point(1, 2, 3);
    EXPECT_FALSE(bbox.empty());
}

TEST(BoundingBox3DTest, scale)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);
    bbox.scale(1, 0.5, 2);

    auto min = bbox.min();
    EXPECT_DOUBLE_EQ(min.x, 0);
    EXPECT_DOUBLE_EQ(min.y, 0.5);
    EXPECT_DOUBLE_EQ(min.z, -1.5);

    auto max = bbox.max();
    EXPECT_DOUBLE_EQ(max.x, 1);
    EXPECT_DOUBLE_EQ(max.y, 1.5);
    EXPECT_DOUBLE_EQ(max.z, 4.5);
}

TEST(BoundingBox3DTest, op_mult_scalar)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);
    bbox *= 0.5;

    auto min = bbox.min();
    EXPECT_DOUBLE_EQ(min.x, 0.25);
    EXPECT_DOUBLE_EQ(min.y, 0.5);
    EXPECT_DOUBLE_EQ(min.z, 0.75);

    auto max = bbox.max();
    EXPECT_DOUBLE_EQ(max.x, 0.75);
    EXPECT_DOUBLE_EQ(max.y, 1.5);
    EXPECT_DOUBLE_EQ(max.z, 2.25);
}

TEST(BoundingBox3DTest, center)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);

    auto ctr = bbox.center();
    EXPECT_DOUBLE_EQ(ctr.x, 0.5);
    EXPECT_DOUBLE_EQ(ctr.y, 1);
    EXPECT_DOUBLE_EQ(ctr.z, 1.5);
}

TEST(BoundingBox3DTest, diag)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);

    auto diag = bbox.diag();
    EXPECT_DOUBLE_EQ(diag, std::sqrt(14));
}

TEST(BoundingBox3DTest, contains)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);

    EXPECT_TRUE(bbox.contains(0.5, 1., 1.));
    EXPECT_TRUE(bbox.contains(Point(0.5, 1., 1.)));

    EXPECT_FALSE(bbox.contains(-1, -1, -1));
    EXPECT_FALSE(bbox.contains(Point(-1, -1, -1)));

    EXPECT_TRUE(bbox.contains(BoundingBox3D(0.1, 0.1, 0.1, 0.9, 1.9, 2.9)));
    EXPECT_FALSE(bbox.contains(BoundingBox3D(-1, -1, -1, 0, 0, 0)));
}

TEST(BoundingBox3DTest, thicken)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);
    bbox.thicken(0.1);

    auto min = bbox.min();
    EXPECT_NEAR(min.x, -0.37416573867739, 1e-14);
    EXPECT_NEAR(min.y, -0.37416573867739, 1e-14);
    EXPECT_NEAR(min.z, -0.37416573867739, 1e-14);

    auto max = bbox.max();
    EXPECT_NEAR(max.x, 1.37416573867739, 1e-14);
    EXPECT_NEAR(max.y, 2.37416573867739, 1e-14);
    EXPECT_NEAR(max.z, 3.37416573867739, 1e-14);
}

TEST(BoundingBox3DTest, reset)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);
    bbox.reset();

    auto min = bbox.min();
    EXPECT_DOUBLE_EQ(min.x, MAX);
    EXPECT_DOUBLE_EQ(min.y, MAX);
    EXPECT_DOUBLE_EQ(min.z, MAX);
    auto max = bbox.max();
    EXPECT_DOUBLE_EQ(max.x, -MAX);
    EXPECT_DOUBLE_EQ(max.y, -MAX);
    EXPECT_DOUBLE_EQ(max.z, -MAX);
}
