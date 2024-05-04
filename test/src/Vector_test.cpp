#include "gmock/gmock.h"
#include "krado/vector.h"

using namespace krado;

TEST(VectorTest, ctor_empty)
{
    Vector vec;
    EXPECT_DOUBLE_EQ(vec.x, 0.);
    EXPECT_DOUBLE_EQ(vec.y, 0.);
    EXPECT_DOUBLE_EQ(vec.z, 0.);
}

TEST(VectorTest, ctor)
{
    Vector vec(1., 2., 3.);
    EXPECT_DOUBLE_EQ(vec.x, 1.);
    EXPECT_DOUBLE_EQ(vec.y, 2.);
    EXPECT_DOUBLE_EQ(vec.z, 3.);
}
