#include "gmock/gmock.h"
#include "krado/helix.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/axis2.h"

using namespace krado;

TEST(HelixTest, ctor1)
{
    Axis2 ctr(Point(0, 0, 0), Vector(1, 0, 0));
    auto helix = Helix::create(ctr, 2., 8., 4);
}
