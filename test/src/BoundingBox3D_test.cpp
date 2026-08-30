#include "gmock/gmock.h"
#include "builder.h"
#include "krado/bounding_box_3d.h"
#include "krado/transform.h"

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
    EXPECT_EQ(min, Point(-1, -2, -3));

    auto max = bbox.max();
    EXPECT_EQ(max, Point(1, 2, 3));
}

TEST(BoundingBox3DTest, empty)
{
    BoundingBox3D bbox;
    EXPECT_TRUE(bbox.empty());

    bbox += Point(1, 2, 3);
    EXPECT_FALSE(bbox.empty());
}

TEST(BoundingBox3DTest, ctor_geom_shape_optimal)
{
    auto box = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));

    BoundingBox3D bbox(box);
    EXPECT_EQ(bbox.min(), Point(0, 0, 0));
    EXPECT_EQ(bbox.max(), Point(1, 2, 3));
}

TEST(BoundingBox3DTest, scale)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);
    bbox.scale(1, 0.5, 2);

    auto min = bbox.min();
    EXPECT_EQ(min, Point(0, 0.5, -1.5));

    auto max = bbox.max();
    EXPECT_EQ(max, Point(1, 1.5, 4.5));
}

TEST(BoundingBox3DTest, op_mult_scalar)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);
    bbox *= 0.5;

    auto min = bbox.min();
    EXPECT_EQ(min, Point(0.25, 0.5, 0.75));

    auto max = bbox.max();
    EXPECT_EQ(max, Point(0.75, 1.5, 2.25));
}

TEST(BoundingBox3DTest, center)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);

    auto ctr = bbox.center();
    EXPECT_EQ(ctr, Point(0.5, 1, 1.5));
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

TEST(BoundingBox3DTest, op_shl)
{
    BoundingBox3D bbox;
    bbox += Point(0, 0, 0);
    bbox += Point(1, 2, 3);

    std::stringstream ss;
    ss << bbox;
    EXPECT_EQ(ss.str(), "BoundingBox: min=(x=0, y=0, z=0), max=(x=1, y=2, z=3)");
}

TEST(BoundingBox3DTest, op_plus_equals)
{
    BoundingBox3D bbox1(Point(1, 1, 1), Point(3, 2, 1));
    BoundingBox3D bbox2(Point(-3, -2, -1), Point(0, 0, 0));
    bbox1 += bbox2;
    auto min = bbox1.min();
    EXPECT_NEAR(min.x, -3., 1e-10);
    EXPECT_NEAR(min.y, -2., 1e-10);
    EXPECT_NEAR(min.z, -1., 1e-10);

    auto max = bbox1.max();
    EXPECT_NEAR(max.x, 3., 1e-10);
    EXPECT_NEAR(max.y, 2., 1e-10);
    EXPECT_NEAR(max.z, 1., 1e-10);
}

TEST(BoundingBox3DTest, transform)
{
    {
        BoundingBox3D bbox(Point(-1, -2, -3), Point(3, 2, 1));
        auto trsf = Trsf::identity() * Trsf::scaled(-3, 2, -4);
        bbox.transform(trsf);
        auto sz = bbox.size();
        EXPECT_NEAR(sz[0], 12., 1e-10);
        EXPECT_NEAR(sz[1], 8., 1e-10);
        EXPECT_NEAR(sz[2], 16., 1e-10);
    }
    {
        BoundingBox3D bbox(Point(-1, -2, -3), Point(3, 4, 2));
        auto trsf = Trsf::identity() * Trsf::translated(1, 2, 3);
        bbox.transform(trsf);
        auto min = bbox.min();
        EXPECT_NEAR(min.x, 0., 1e-10);
        EXPECT_NEAR(min.y, 0., 1e-10);
        EXPECT_NEAR(min.z, 0., 1e-10);

        auto max = bbox.max();
        EXPECT_NEAR(max.x, 4., 1e-10);
        EXPECT_NEAR(max.y, 6., 1e-10);
        EXPECT_NEAR(max.z, 5., 1e-10);
    }
}

TEST(BoundingBox3DTest, determine_spatial_dim)
{
    {
        BoundingBox3D bbox(Point(1, 0, 0), Point(2, 0, 0));
        auto dim = determine_spatial_dim(bbox);
        ASSERT_TRUE(dim.has_value());
        EXPECT_EQ(dim.value(), 1);
    }
    {
        BoundingBox3D bbox(Point(1, 0, 0), Point(2, 2, 0));
        auto dim = determine_spatial_dim(bbox);
        ASSERT_TRUE(dim.has_value());
        EXPECT_EQ(dim.value(), 2);
    }
    {
        BoundingBox3D bbox(Point(1, 0, 0), Point(2, 2, 3));
        auto dim = determine_spatial_dim(bbox);
        ASSERT_TRUE(dim.has_value());
        EXPECT_EQ(dim.value(), 3);
    }
    {
        BoundingBox3D bbox(Point(1, 0, 0), Point(2, 0, 3));
        auto dim = determine_spatial_dim(bbox);
        EXPECT_FALSE(dim.has_value());
    }
}

TEST(BoundingBox3DTest, make_cube)
{
    BoundingBox3D bbox(Point(-2, -3, -4), Point(4, 3, 2));
    bbox.make_cube();

    auto min = bbox.min();
    EXPECT_NEAR(min.x, -4.196152, 1e-6);
    EXPECT_NEAR(min.y, -5.196152, 1e-6);
    EXPECT_NEAR(min.z, -6.196152, 1e-6);

    auto max = bbox.max();
    EXPECT_NEAR(max.x, 6.196152, 1e-6);
    EXPECT_NEAR(max.y, 5.196152, 1e-6);
    EXPECT_NEAR(max.z, 4.196152, 1e-6);
}
