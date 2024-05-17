#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include "krado/export.h"
#include "builder.h"

using namespace krado;

TEST(WriteExodusIITest, line)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    Mesh mesh(model);

    auto & line = mesh.curve(3);
    // clang-format off
    line.set_scheme("bias")
        .set("intervals", 5)
        .set("coef", 1.2);
    // clang-format on
    mesh.mesh_curve(3);

    mesh.number_points();
    mesh.build_elements();

    write_exodusii(mesh, "line.exo");
}
