#include "gmock/gmock.h"
#include "krado/line.h"

using namespace krado;

TEST(LineTest, test)
{
    auto ln = Line::create(Point(0, 0, 0), Point(1, 2, 3));
    EXPECT_DOUBLE_EQ(ln.length(), std::sqrt(1 + 4 + 9));
}
