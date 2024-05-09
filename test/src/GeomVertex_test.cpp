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
}
