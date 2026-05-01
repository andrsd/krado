#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/tricircle.h"

using namespace krado;

TEST(SchemeCircleTest, circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    SchemeEqual::Options opts1;
    opts1.intervals = 8;
    model.curve(1)->set_scheme<SchemeEqual>(opts1);
    model.mesh_curve(1);

    auto surf = model.surface(1);
    SchemeTriCircle::Options opts;
    opts.radial_intervals = 1;
    surf->set_scheme<SchemeTriCircle>(opts);
    model.mesh_surface(1);

    auto SQRT2_2 = std::sqrt(2.) / 2.;

    auto & sv = surf->surface_vertices();
    ASSERT_EQ(sv.size(), 1);
    EXPECT_EQ(sv[0]->point(), Point(0, 0, 0));

    auto crv = model.curve(1);
    auto cv = crv->curve_vertices();
    EXPECT_TRUE(cv[0]->point().is_equal(Point(SQRT2_2, SQRT2_2, 0), 1e-5));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0, 1., 0), 1e-5));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(-SQRT2_2, SQRT2_2, 0), 1e-5));
    EXPECT_TRUE(cv[3]->point().is_equal(Point(-1., 0, 0), 1e-5));
    EXPECT_TRUE(cv[4]->point().is_equal(Point(-SQRT2_2, -SQRT2_2, 0), 1e-5));
    EXPECT_TRUE(cv[5]->point().is_equal(Point(0, -1., 0), 1e-5));
    EXPECT_TRUE(cv[6]->point().is_equal(Point(SQRT2_2, -SQRT2_2, 0), 1e-5));

    EXPECT_EQ(surf->triangles().size(), 8);
}

TEST(SchemeCircleTest, not_circle)
{
    auto circle = testing::build_rect(Point(0, 0, 0), Point(1, 2, 3));
    GeomModel model(circle);

    auto surf = model.surface(1);
    SchemeTriCircle::Options opts;
    surf->set_scheme<SchemeTriCircle>(opts);
    EXPECT_THROW_MSG(model.mesh_surface(1), "Surface 1 is not a circle");
}
