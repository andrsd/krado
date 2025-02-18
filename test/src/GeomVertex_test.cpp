#include "gmock/gmock.h"
#include "krado/geom_vertex.h"
#include "builder.h"

using namespace krado;

TEST(GeomVertexTest, ctor)
{
    auto vtx = testing::build_vertex(Point(1, 2, 3));
    GeomVertex v(vtx);
    EXPECT_DOUBLE_EQ(v.x(), 1.);
    EXPECT_DOUBLE_EQ(v.y(), 2.);
    EXPECT_DOUBLE_EQ(v.z(), 3.);

    EXPECT_FALSE(v.is_null());
}

TEST(GeomVertexTest, null_vertex)
{
    TopoDS_Vertex vtx;

    GeomVertex v(vtx);
    EXPECT_TRUE(v.is_null());
}

TEST(GeomVertexTest, point)
{
    auto vtx = testing::build_vertex(Point(1, 2, 3));
    GeomVertex v(vtx);
    auto pt = v.point();
    EXPECT_DOUBLE_EQ(pt.x, 1.);
    EXPECT_DOUBLE_EQ(pt.y, 2.);
    EXPECT_DOUBLE_EQ(pt.z, 3.);
}
