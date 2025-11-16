#include "gmock/gmock.h"
#include "krado/cylinder.h"

using namespace krado;

TEST(CylinderTest, test)
{
    Axis2 ax2(Point(0, 0, 0), Vector(1, 0, 0));
    Cylinder cyl(ax2, 2., 3);
    EXPECT_DOUBLE_EQ(cyl.volume(), M_PI * 4 * 3);
}
