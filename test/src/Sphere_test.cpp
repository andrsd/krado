#include "gmock/gmock.h"
#include "krado/sphere.h"

using namespace krado;

TEST(SphereTest, test)
{
    Sphere sp(Point(0, 0, 0), 2);
    EXPECT_DOUBLE_EQ(sp.volume(), 4. / 3. * M_PI * 8.);
}
