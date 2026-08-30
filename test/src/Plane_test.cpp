#include "gmock/gmock.h"
#include "krado/plane.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/axis1.h"

using namespace krado;

TEST(PlaneTest, ctor_occt)
{
    Point center(1, 2, 3);
    gp_Ax2 center_ax(center, gp_Dir(0, 0, 1));
    gp_Pln qp_plane(center_ax);
    Plane pln(qp_plane);

    auto loc = pln.location();
    EXPECT_DOUBLE_EQ(loc.x, 1.);
    EXPECT_DOUBLE_EQ(loc.y, 2.);
    EXPECT_DOUBLE_EQ(loc.z, 3.);

    auto ax = pln.axis();
    EXPECT_DOUBLE_EQ(ax.direction().x, 0.);
    EXPECT_DOUBLE_EQ(ax.direction().y, 0.);
    EXPECT_DOUBLE_EQ(ax.direction().z, 1.);

    auto axx = pln.x_axis();
    EXPECT_DOUBLE_EQ(axx.direction().x, 1.);
    EXPECT_DOUBLE_EQ(axx.direction().y, 0.);
    EXPECT_DOUBLE_EQ(axx.direction().z, 0.);

    auto axy = pln.y_axis();
    EXPECT_DOUBLE_EQ(axy.direction().x, 0.);
    EXPECT_DOUBLE_EQ(axy.direction().y, 1.);
    EXPECT_DOUBLE_EQ(axy.direction().z, 0.);
}

TEST(PlaneTest, ctor_kr)
{
    Point center(1, 2, 3);
    Vector n(0, 0, 1);
    Plane pln(center, n);

    auto loc = pln.location();
    EXPECT_DOUBLE_EQ(loc.x, 1.);
    EXPECT_DOUBLE_EQ(loc.y, 2.);
    EXPECT_DOUBLE_EQ(loc.z, 3.);

    auto ax = pln.axis();
    EXPECT_DOUBLE_EQ(ax.direction().x, 0.);
    EXPECT_DOUBLE_EQ(ax.direction().y, 0.);
    EXPECT_DOUBLE_EQ(ax.direction().z, 1.);

    auto axx = pln.x_axis();
    EXPECT_DOUBLE_EQ(axx.direction().x, 1.);
    EXPECT_DOUBLE_EQ(axx.direction().y, 0.);
    EXPECT_DOUBLE_EQ(axx.direction().z, 0.);

    auto axy = pln.y_axis();
    EXPECT_DOUBLE_EQ(axy.direction().x, 0.);
    EXPECT_DOUBLE_EQ(axy.direction().y, 1.);
    EXPECT_DOUBLE_EQ(axy.direction().z, 0.);
}
