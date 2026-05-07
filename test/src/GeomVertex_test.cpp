#include "gmock/gmock.h"
#include "krado/geom_vertex.h"
#include "builder.h"

using namespace krado;

TEST(GeomVertexTest, ctor)
{
    auto v = testing::build_vertex(Point(1, 2, 3));
    EXPECT_DOUBLE_EQ(v.x(), 1.);
    EXPECT_DOUBLE_EQ(v.y(), 2.);
    EXPECT_DOUBLE_EQ(v.z(), 3.);

    EXPECT_FALSE(v.is_null());
}

TEST(GeomVertexTest, dim)
{
    auto v = testing::build_vertex(Point(1, 2, 3));
    EXPECT_EQ(v.dim(), 0);
}

TEST(GeomVertexTest, null_vertex)
{
    TopoDS_Vertex vtx;

    GeomVertex v(vtx);
    EXPECT_TRUE(v.is_null());
}

TEST(GeomVertexTest, point)
{
    auto v = testing::build_vertex(Point(1, 2, 3));
    auto pt = v.point();
    EXPECT_DOUBLE_EQ(pt.x, 1.);
    EXPECT_DOUBLE_EQ(pt.y, 2.);
    EXPECT_DOUBLE_EQ(pt.z, 3.);
}

TEST(GeomVertexTest, op_shl)
{
    auto vtx = testing::build_vertex(Point(1, 2, 3));
    std::stringstream ss;
    ss << vtx;
    EXPECT_EQ(ss.str(), "Vertex: location=(x=1, y=2, z=3)");
}
