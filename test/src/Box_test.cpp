#include "gmock/gmock.h"
#include "krado/box.h"

using namespace krado;

TEST(BoxTest, test)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    EXPECT_DOUBLE_EQ(box.volume(), 6.);
}
