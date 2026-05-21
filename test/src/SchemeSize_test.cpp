#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "builder.h"
#include "krado/scheme/size.h"

using namespace krado;

TEST(SchemeSizeTest, line_0_3)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    SchemeSize::Options opts;
    opts.size = 0.3;
    model.curve(1)->set_scheme<SchemeSize>(opts);
    model.mesh_curve(1);

    auto line = model.curve(1);
    auto bv = line->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(0, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto cv = line->curve_vertices();
    ASSERT_EQ(cv.size(), 2);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(0.333333, 0, 0), 1e-5));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0.666666, 0, 0), 1e-5));

    ASSERT_EQ(line->segments().size(), 3);
}

TEST(SchemeSizeTest, line_0_15)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    SchemeSize::Options opts;
    opts.size = 0.15;
    model.curve(1)->set_scheme<SchemeSize>(opts);
    model.mesh_curve(1);

    auto line = model.curve(1);
    auto bv = line->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(0, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto cv = line->curve_vertices();
    ASSERT_EQ(cv.size(), 6);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(0.142857, 0, 0), 1e-5));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0.285714, 0, 0), 1e-5));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(0.428571, 0, 0), 1e-5));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(0.571429, 0, 0), 1e-5));
    EXPECT_TRUE(cv[4]->point().is_equal(Point(0.714286, 0, 0), 1e-5));
    EXPECT_TRUE(cv[5]->point().is_equal(Point(0.857143, 0, 0), 1e-5));

    ASSERT_EQ(line->segments().size(), 7);
}
