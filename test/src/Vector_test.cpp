#include "gmock/gmock.h"
#include "krado/vector.h"
#include "krado/exception.h"

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

TEST(VectorTest, op_call)
{
    Vector vec(5., 6., 9.);
    EXPECT_DOUBLE_EQ(vec(0), 5.);
    EXPECT_DOUBLE_EQ(vec(1), 6.);
    EXPECT_DOUBLE_EQ(vec(2), 9.);

    EXPECT_THROW({vec(10);}, Exception);
}
