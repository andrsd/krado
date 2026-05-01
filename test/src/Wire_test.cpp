#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "krado/wire.h"
#include "krado/point.h"
#include "krado/line.h"
#include "krado/exception.h"

using namespace krado;

namespace {

double
radians(double deg)
{
    return deg * M_PI / 180.;
}

} // namespace

TEST(WireTest, empty)
{
    std::vector<GeomCurve> edges;
    EXPECT_THROW({ Wire::create(edges); }, Exception);
}

TEST(WireTest, wire)
{
    Point pt1(0, 0, 0);
    Point pt2(1, 0, 0);
    Point pt3(1, 1, 0);
    auto ln1 = Line::create(pt1, pt2);
    auto ln2 = Line::create(pt2, pt3);
    auto wire = Wire::create({ ln1, ln2 });
}

TEST(WireTest, length)
{
    Point pt1(0, 0, 0);
    Point pt2(1, 0, 0);
    Point pt3(1, 1, 0);
    auto ln1 = Line::create(pt1, pt2);
    auto ln2 = Line::create(pt2, pt3);
    auto wire = Wire::create({ ln1, ln2 });
    EXPECT_DOUBLE_EQ(wire.length(), 2.);
}
