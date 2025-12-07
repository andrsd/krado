#include "gmock/gmock.h"
#include "krado/inscribed_polygon.h"
#include "krado/axis2.h"
#include "krado/exception.h"

using namespace krado;

TEST(InscribedPolygonTest, ctor1)
{
    Axis2 ax2(Point(1, 1, 0), Vector(0, 0, 1));
    auto plgn = InscribedPolygon::create(ax2, 2, 3);

    EXPECT_THROW({ InscribedPolygon::create(ax2, 2, 1); }, Exception);
}

TEST(InscribedPolygonTest, ctor2)
{
    Axis2 ax2(Point(1, 1, 0), Vector(0, 0, 1));
    auto plgn = InscribedPolygon::create(ax2, Point(0, 1, 0), 3);

    EXPECT_THROW({ InscribedPolygon::create(ax2, Point(0, 1, 0), 1); }, Exception);
}
