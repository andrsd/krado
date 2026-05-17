#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/scheme/equal.h"
#include "krado/step_file.h"
#include "builder.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(SchemeEqualTest, line)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    SchemeEqual::Options opts;
    opts.intervals = 5;
    model.curve(1)->set_scheme<SchemeEqual>(opts);
    model.mesh_curve(1);

    auto line = model.curve(1);
    auto bv = line->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(0, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto cv = line->curve_vertices();
    ASSERT_EQ(cv.size(), 4);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(0.2, 0, 0), 1e-10));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0.4, 0, 0), 1e-10));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(0.6, 0, 0), 1e-10));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(0.8, 0, 0), 1e-10));

    ASSERT_EQ(line->segments().size(), 5);
}

TEST(SchemeEqualTest, circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    SchemeEqual::Options opts;
    opts.intervals = 8;
    model.curve(1)->set_scheme<SchemeEqual>(opts);
    model.mesh_curve(1);

    auto curv = model.curve(1);

    auto SQRT2_2 = std::sqrt(2.) / 2.;

    auto bv = curv->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(1, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto cv = curv->curve_vertices();
    ASSERT_EQ(cv.size(), 7);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(SQRT2_2, SQRT2_2, 0), 1e-10));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0, 1., 0), 1e-10));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(-SQRT2_2, SQRT2_2, 0), 1e-10));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(-1., 0, 0), 1e-10));
    EXPECT_TRUE(cv[4]->point().is_equal(Point(-SQRT2_2, -SQRT2_2, 0), 1e-10));
    EXPECT_TRUE(cv[5]->point().is_equal(Point(0, -1., 0), 1e-10));
    EXPECT_TRUE(cv[6]->point().is_equal(Point(SQRT2_2, -SQRT2_2, 0), 1e-10));

    EXPECT_EQ(curv->segments().size(), 8);
}

TEST(SchemeEqualTest, quarter_circle)
{
    fs::path input_file =
        fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "geo" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.read();
    auto shape = shapes[0];
    GeomModel model(shape);

    SchemeEqual::Options opts1;
    opts1.intervals = 4;
    model.curve(1)->set_scheme<SchemeEqual>(opts1);
    model.mesh_curve(1);

    auto curv = model.curve(1);
    auto bv = curv->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(-1, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(0, 1, 0), 1e-10));

    auto cv = curv->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_TRUE(
        cv[0]->point().is_equal(Point(-std::cos(M_PI / 8.), std::sin(M_PI / 8.), 0), 1e-10));
    EXPECT_TRUE(
        cv[1]->point().is_equal(Point(-std::cos(M_PI / 4.), std::sin(M_PI / 4.), 0), 1e-10));
    EXPECT_TRUE(
        cv[2]->point().is_equal(Point(-std::cos(3 * M_PI / 8.), std::sin(3. * M_PI / 8.), 0),
                                1e-10));
}
