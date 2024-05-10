#include "gmock/gmock.h"
#include "krado/scheme_equal.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include "builder.h"

using namespace krado;

TEST(SchemeEqualTest, line)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(2, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("equal")
        .set("intervals", 4);
    // clang-format on
    mesh.mesh_curve(3);

    ASSERT_EQ(line.all_vertices().size(), 5);
    auto first_vtx = line.all_vertices().front();
    auto last_vtx = line.all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line.segments().size(), 4);
}

TEST(SchemeEqualTest, circle)
{
    auto shape = GeomShape(testing::build_circle(Point(0, 0, 0), 2));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & circ = mesh.curve(2);
    // clang-format off
    circ.set_scheme("equal")
        .set("intervals", 20);
    // clang-format on
    mesh.mesh_curve(2);

    EXPECT_EQ(circ.get_scheme().name(), "equal");

    ASSERT_EQ(circ.all_vertices().size(), 21);
    auto first_vtx = circ.all_vertices().front();
    auto last_vtx = circ.all_vertices().back();
    EXPECT_EQ(first_vtx, last_vtx);

    ASSERT_EQ(circ.segments().size(), 20);
}
