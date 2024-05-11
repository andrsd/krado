#include "gmock/gmock.h"
#include "krado/scheme_transfinite.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include "krado/mesh_curve_vertex.h"
#include "builder.h"

TEST(SchemeTransfiniteTest, transfinite_1d_uniform)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("transfinite")
        .set("intervals", 5)
        .set("coef", 1.)
        .set("type", std::string("progression"));
    // clang-format on
    mesh.mesh_curve(3);

    ASSERT_EQ(line.all_vertices().size(), 6);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.curve_vertices().size(), 4);
    auto & cv = line.curve_vertices();
    EXPECT_DOUBLE_EQ(cv[0]->point().x, 0.2);
    EXPECT_DOUBLE_EQ(cv[1]->point().x, 0.4);
    EXPECT_DOUBLE_EQ(cv[2]->point().x, 0.6);
    EXPECT_DOUBLE_EQ(cv[3]->point().x, 0.8);

    ASSERT_EQ(line.segments().size(), 5);
}

TEST(SchemeTransfiniteTest, transfinite_1d_bias)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("transfinite")
        .set("intervals", 5)
        .set("coef", 1.2)
        .set("type", std::string("progression"));
    // clang-format on
    mesh.mesh_curve(3);

    ASSERT_EQ(line.all_vertices().size(), 6);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.curve_vertices().size(), 4);
    auto & cv = line.curve_vertices();
    EXPECT_NEAR(cv[0]->point().x, 0.13438, 1e-6);
    EXPECT_NEAR(cv[1]->point().x, 0.295635, 1e-6);
    EXPECT_NEAR(cv[2]->point().x, 0.489142, 1e-6);
    EXPECT_NEAR(cv[3]->point().x, 0.72135, 1e-6);

    ASSERT_EQ(line.segments().size(), 5);
}

TEST(SchemeTransfiniteTest, transfinite_1d_bump_coef_above_one)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("transfinite")
        .set("intervals", 5)
        .set("coef", 1.2)
        .set("type", std::string("bump"));
    // clang-format on
    mesh.mesh_curve(3);

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

TEST(SchemeTransfiniteTest, transfinite_1d_bump_coef_below_one)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("transfinite")
        .set("intervals", 5)
        .set("coef", 0.8)
        .set("type", std::string("bump"));
    // clang-format on
    mesh.mesh_curve(3);

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

TEST(SchemeTransfiniteTest, transfinite_1d_beta_law)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("transfinite")
        .set("intervals", 5)
        .set("coef", 1.2)
        .set("type", std::string("beta-law"));
    // clang-format on
    mesh.mesh_curve(3);

    ASSERT_EQ(line.all_vertices().size(), 6);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.curve_vertices().size(), 4);
    auto & cv = line.curve_vertices();
    EXPECT_NEAR(cv[0]->point().x, 0.107319, 1e-6);
    EXPECT_NEAR(cv[1]->point().x, 0.260179, 1e-6);
    EXPECT_NEAR(cv[2]->point().x, 0.464912, 1e-6);
    EXPECT_NEAR(cv[3]->point().x, 0.717644, 1e-6);

    ASSERT_EQ(line.segments().size(), 5);
}

TEST(SchemeTransfiniteTest, transfinite_1d_invalid_scheme)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("transfinite")
        .set("intervals", 5)
        .set("coef", 1.2)
        .set("type", std::string("invalid"));
    // clang-format on
    EXPECT_THROW({mesh.mesh_curve(3);}, Exception);
}
