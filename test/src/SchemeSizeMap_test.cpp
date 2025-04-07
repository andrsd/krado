#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve_vertex.h"
#include "builder.h"

using namespace krado;

TEST(SchemeSizemapTest, line)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    // clang-format off
    model.vertex(1)
        .set_mesh_size(0.4);
    model.vertex(2)
        .set_mesh_size(0.1);
    model.curve(1)
        .set_scheme("sizemap")
        .set("intervals", 5);
    // clang-format on
    model.mesh_curve(1);

    auto & line = model.curve(1);
    ASSERT_EQ(line.all_vertices().size(), 6);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.curve_vertices().size(), 4);
    auto & cv = line.curve_vertices();
    EXPECT_NEAR(cv[0]->point().x, 0.32, 1e-1);
    EXPECT_NEAR(cv[1]->point().x, 0.56, 1e-1);
    EXPECT_NEAR(cv[2]->point().x, 0.75, 1e-1);
    EXPECT_NEAR(cv[3]->point().x, 0.89, 1e-1);

    ASSERT_EQ(line.segments().size(), 5);
}
