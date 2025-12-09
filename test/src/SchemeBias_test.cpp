#include "gmock/gmock.h"
#include <iomanip>
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/geom_model.h"
#include "krado/geom_curve.h"
#include "krado/scheme/bias.h"
#include "builder.h"

using namespace krado;

TEST(SchemeBiasTest, line)
{
    auto line = testing::build_line(Point(1, 0, 0), Point(2, 0, 0));

    GeomModel model(line);

    SchemeBias::Options opts;
    opts.intervals = 5;
    opts.factor = 1.1;
    model.curve(1)->set_scheme<SchemeBias>(opts);
    model.mesh_curve(1);

    auto mline = model.curve(1);
    ASSERT_EQ(mline->all_vertices().size(), 6);
    auto first_vtx = mline->all_vertices().front();
    auto last_vtx = mline->all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    auto & cv = mline->curve_vertices();
    ASSERT_EQ(cv.size(), 4);
    EXPECT_NEAR(cv[0]->point().x, 1.1637975, 1e-6);
    EXPECT_NEAR(cv[1]->point().x, 1.3439747, 1e-6);
    EXPECT_NEAR(cv[2]->point().x, 1.5421697, 1e-6);
    EXPECT_NEAR(cv[3]->point().x, 1.7601841, 1e-6);

    ASSERT_EQ(mline->segments().size(), 5);
}

TEST(SchemeBiasTest, arc)
{
    auto arc = testing::build_arc();

    GeomModel model(arc);

    SchemeBias::Options opts;
    opts.intervals = 5;
    opts.factor = 1.1;
    model.curve(1)->set_scheme<SchemeBias>(opts);
    model.mesh_curve(1);

    auto mline = model.curve(1);
    ASSERT_EQ(mline->all_vertices().size(), 6);
    auto first_vtx = mline->all_vertices().front();
    auto last_vtx = mline->all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    auto & cv = mline->all_vertices();
    ASSERT_EQ(cv.size(), 6);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(-1., 0., 0), 1e-6));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(-0.870497, 0.4921736, 0), 1e-6));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(-0.470774, 0.8822538, 0), 1e-6));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(0.1320927, 0.9912374, 0), 1e-6));
    EXPECT_TRUE(cv[4]->point().is_equal(Point(0.7293644, 0.6841254, 0), 1e-6));
    EXPECT_TRUE(cv[5]->point().is_equal(Point(1., 0., 0), 1e-6));

    ASSERT_EQ(mline->segments().size(), 5);
}

TEST(SchemeBiasTest, circle)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);

    GeomModel model(circ);

    SchemeBias::Options opts;
    opts.intervals = 5;
    opts.factor = 1.1;
    model.curve(1)->set_scheme<SchemeBias>(opts);
    model.mesh_curve(1);

    auto mline = model.curve(1);
    auto & cv = mline->all_vertices();
    ASSERT_EQ(cv.size(), 5);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(2, 0, 0)));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(1.031061, 1.713743, 0), 1e-6));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(-1.113487, 1.661369, 0), 1e-6));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(-1.930206, -0.5237409, 0), 1e-6));
    EXPECT_TRUE(cv[4]->point().is_equal(Point(0.12789, -1.995907, 0), 1e-6));

    ASSERT_EQ(mline->segments().size(), 5);
}
