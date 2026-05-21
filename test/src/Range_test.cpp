#include <gmock/gmock.h>
#include "krado/range.h"

using namespace krado;

TEST(RangeTest, test)
{
    Range rng(2, 10);
    EXPECT_EQ(rng.first(), 2);
    EXPECT_EQ(rng.last(), 10);
    EXPECT_EQ(rng.size(), 8);
}

TEST(RangeTest, make_range_1)
{
    auto rng = make_range(7);
    EXPECT_EQ(rng.first(), 0);
    EXPECT_EQ(rng.last(), 7);
}

TEST(RangeTest, make_range_2)
{
    auto rng = make_range(2, 7);
    EXPECT_EQ(rng.first(), 2);
    EXPECT_EQ(rng.last(), 7);
}
