#include "gmock/gmock.h"
#include "krado/circumscribed_polygon.h"
#include "krado/axis2.h"
#include "krado/exception.h"

using namespace krado;

TEST(CirsumscribedPolygonTest, ctor1)
{
    Axis2 ax2(Point(1, 1, 0), Vector(0, 0, 1));
    auto plgn = CircumscribedPolygon::create(ax2, 2, 3);

    EXPECT_THROW({ CircumscribedPolygon::create(ax2, 2, 1); }, Exception);
}

TEST(CirsumscribedPolygonTest, ctor2)
{
    Axis2 ax2(Point(1, 1, 0), Vector(0, 0, 1));
    auto plgn = CircumscribedPolygon::create(ax2, Point(0, 1, 0), 3);

    EXPECT_THROW({ CircumscribedPolygon::create(ax2, Point(0, 1, 0), 1); }, Exception);
}
