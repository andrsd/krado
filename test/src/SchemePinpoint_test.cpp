#include "gmock/gmock.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/geom_model.h"
#include "krado/geom_curve.h"
#include "krado/scheme/pinpoint.h"
#include "builder.h"

using namespace krado;

TEST(SchemePinpointTest, line)
{
    auto line = testing::build_line(Point(1, 0, 0), Point(2, 0, 0));

    GeomModel model(line);

    SchemePinpoint::Options opts;
    opts.positions = { 0.1, 0.4, 0.75 };
    model.curve(1)->set_scheme<SchemePinpoint>(opts);
    model.mesh_curve(1);

    auto mline = model.curve(1);
    auto & bv = mline->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(1, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(2, 0, 0), 1e-10));

    auto & cv = mline->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(1.1, 0, 0), 1e-6));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(1.4, 0, 0), 1e-6));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(1.75, 0, 0), 1e-6));

    auto & segs = mline->segments();
    ASSERT_EQ(segs.size(), 4);
}

TEST(SchemePinpointTest, circle)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);

    GeomModel model(circ);

    SchemePinpoint::Options opts;
    opts.positions = { 0.1, 0.4, 0.75 };
    model.curve(1)->set_scheme<SchemePinpoint>(opts);
    model.mesh_curve(1);

    auto mline = model.curve(1);
    auto & bv = mline->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(2, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(2, 0, 0), 1e-10));

    auto & cv = mline->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(1.997501, 0.0999583, 0), 1e-6));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(1.960133, 0.3973387, 0), 1e-6));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(1.861015, 0.7325451, 0), 1e-6));

    auto & segs = mline->segments();
    ASSERT_EQ(segs.size(), 4);
}
