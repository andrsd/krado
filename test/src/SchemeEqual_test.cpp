#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_model.h"
#include "krado/mesh_curve_vertex.h"
#include "builder.h"

using namespace krado;

TEST(SchemeEqualTest, line)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    MeshModel mesh(model);

    auto & line = mesh.curve(1);
    // clang-format off
    line.set_scheme("equal")
        .set("intervals", 5);
    // clang-format on
    mesh.mesh_curve(1);

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
