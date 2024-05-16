#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include "krado/mesh_curve_vertex.h"
#include "builder.h"

using namespace krado;

TEST(SchemeBetaLawTest, test)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("beta-law")
        .set("intervals", 5)
        .set("coef", 1.2);
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
