#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve_vertex.h"
#include "builder.h"

using namespace krado;

TEST(SchemeBumpTest, coef_above_one)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);

    auto & curve = model.curve(1);
    // clang-format off
    curve.set_scheme("bump")
         .set("intervals", 5)
         .set("coef", 1.2);
    // clang-format on
    model.mesh_curve(1);

    auto & line = model.curve(1);
    ASSERT_EQ(line.all_vertices().size(), 6);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.curve_vertices().size(), 4);
    auto & cv = line.curve_vertices();
    EXPECT_NEAR(cv[0]->point().x, 0.211754, 1e-6);
    EXPECT_NEAR(cv[1]->point().x, 0.405743, 1e-6);
    EXPECT_NEAR(cv[2]->point().x, 0.594257, 1e-6);
    EXPECT_NEAR(cv[3]->point().x, 0.788246, 1e-6);

    ASSERT_EQ(line.segments().size(), 5);
}

TEST(SchemeBumpTest, coef_below_one)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);

    auto & curve = model.curve(1);
    // clang-format off
    curve.set_scheme("bump")
         .set("intervals", 5)
         .set("coef", 0.8);
    // clang-format on
    model.mesh_curve(1);

    auto & line = model.curve(1);
    ASSERT_EQ(line.all_vertices().size(), 6);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.curve_vertices().size(), 4);
    auto & cv = line.curve_vertices();
    EXPECT_NEAR(cv[0]->point().x, 0.185874, 1e-6);
    EXPECT_NEAR(cv[1]->point().x, 0.392729, 1e-6);
    EXPECT_NEAR(cv[2]->point().x, 0.607271, 1e-6);
    EXPECT_NEAR(cv[3]->point().x, 0.814126, 1e-6);

    ASSERT_EQ(line.segments().size(), 5);
}
