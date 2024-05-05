#include "gmock/gmock.h"
#include "krado/mesh_point.h"

using namespace krado;

TEST(MeshPointTest, api)
{
    MeshPoint mpoint(1, 2, 3);
    EXPECT_DOUBLE_EQ(mpoint.x, 1.);
    EXPECT_DOUBLE_EQ(mpoint.y, 2.);
    EXPECT_DOUBLE_EQ(mpoint.z, 3.);
}
