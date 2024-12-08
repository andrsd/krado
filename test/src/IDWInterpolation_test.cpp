#include "gmock/gmock.h"
#include "krado/idw_interpolation.h"

using namespace krado;

TEST(IDWInterpolationTest, sample)
{
    IDWInterpolation idw({ Point(0, 0), Point(1, 0) }, { 1, 2 });
    EXPECT_NEAR(idw.sample(Point(0.5, 0)), 1.5, 1e-15);
}
