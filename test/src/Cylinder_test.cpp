#include "gmock/gmock.h"
#include "krado/cylinder.h"
#include "krado/axis2.h"
#include "krado/vector.h"
#include "krado/point.h"

using namespace krado;

TEST(CylinderTest, test)
{
    Axis2 ax2(Point(0, 0, 0), Vector(1, 0, 0));
    auto cyl = Cylinder::create(ax2, 2., 3);
    EXPECT_DOUBLE_EQ(cyl.volume(), M_PI * 4 * 3);
}
