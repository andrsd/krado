#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_vertex.h"
#include "krado/scheme/curvature.h"
#include "krado/step_file.h"
#include "krado/spline.h"
#include "krado/utils.h"
#include "builder.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(SchemeCurvatureTest, line)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    SchemeCurvature::Options opts;
    opts.min_size = 0.1;
    opts.max_size = 0.2;
    opts.deflection = 0.1;
    model.curve(1)->set_scheme<SchemeCurvature>(opts);
    model.mesh_curve(1);

    auto line = model.curve(1);
    // kappa = 0, so h = max_size = 0.2.
    // Length 1.0 / 0.2 = 5 intervals.
    ASSERT_EQ(line->segments().size(), 5);

    auto & bv = line->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(0, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto & cv = line->curve_vertices();
    ASSERT_EQ(cv.size(), 4);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(0.2, 0, 0), 1e-10));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0.4, 0, 0), 1e-10));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(0.6, 0, 0), 1e-10));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(0.8, 0, 0), 1e-10));
}

TEST(SchemeCurvatureTest, quarter_circle)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "geo" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    SchemeCurvature::Options opts;
    opts.min_size = 0.01;
    opts.max_size = 1.0;
    opts.deflection = M_PI / 8.; // 22.5 degrees
    model.curve(1)->set_scheme<SchemeCurvature>(opts);
    model.mesh_curve(1);

    auto curv = model.curve(1);
    // quarter circle has angle PI/2.
    // deflection is PI/8.
    // We expect PI/2 / (PI/8) = 4 intervals.
    ASSERT_EQ(curv->segments().size(), 4);

    auto & bv = curv->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(-1, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(0, 1, 0), 1e-10));

    auto & cv = curv->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(-0.92388, 0.382683, 0), 1e-6));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(-0.707107, 0.707107, 0), 1e-6));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(-0.382683, 0.92388, 0), 1e-6));
}

TEST(SchemeCurvatureTest, circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    SchemeCurvature::Options opts;
    opts.min_size = 0.01;
    opts.max_size = 1.0;
    opts.deflection = M_PI / 4.; // 45 degrees
    model.curve(1)->set_scheme<SchemeCurvature>(opts);
    model.mesh_curve(1);

    auto curv = model.curve(1);
    // full circle has angle 2*PI.
    // deflection is PI/4.
    // We expect 2*PI / (PI/4) = 8 intervals.
    ASSERT_EQ(curv->segments().size(), 8);

    auto & bv = curv->bounding_vertices();
    ASSERT_EQ(bv.size(), 1);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto & cv = curv->curve_vertices();
    ASSERT_EQ(cv.size(), 7);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(0.707107, 0.707107, 0), 1e-5));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0., 1, 0), 1e-5));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(-0.707107, 0.707107, 0), 1e-5));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(-1, 0, 0), 1e-10));
    EXPECT_TRUE(cv[4]->point().is_equal(Point(-0.707107, -0.707107, 0), 1e-5));
    EXPECT_TRUE(cv[5]->point().is_equal(Point(0, -1, 0), 1e-10));
    EXPECT_TRUE(cv[6]->point().is_equal(Point(0.707107, -0.707107, 0), 1e-5));
}

TEST(SchemeCurvatureTest, spline)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(1, 1, 0), Point(2, 0, 0) };
    auto spline = Spline::create(pts);
    GeomModel model(spline);

    SchemeCurvature::Options opts;
    opts.min_size = 0.05;
    opts.max_size = 0.5;
    opts.deflection = 0.1;
    model.curve(1)->set_scheme<SchemeCurvature>(opts);
    model.mesh_curve(1);

    auto curv = model.curve(1);
    // Just check that it meshed and has more than 1 segment
    ASSERT_GT(curv->segments().size(), 1);

    auto & bv = curv->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(0, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(2, 0, 0), 1e-10));

    auto & cv = curv->curve_vertices();
    ASSERT_EQ(cv.size(), 22);
    EXPECT_TRUE(cv[3]->point().is_equal(Point(0.643486, 0.872898, 0), 1e-5));
    EXPECT_TRUE(cv[7]->point().is_equal(Point(0.904346, 0.99085, 0), 1e-5));
    EXPECT_TRUE(cv[8]->point().is_equal(Point(0.95659, 0.998116, 0), 1e-5));
    EXPECT_TRUE(cv[9]->point().is_equal(Point(1.0071, 0.99995, 0), 1e-5));
    EXPECT_TRUE(cv[10]->point().is_equal(Point(1.05699, 0.996752, 0), 1e-5));
    EXPECT_TRUE(cv[16]->point().is_equal(Point(1.4037, 0.837029, 0), 1e-5));
    EXPECT_TRUE(cv[19]->point().is_equal(Point(1.68545, 0.530162, 0), 1e-5));
}
