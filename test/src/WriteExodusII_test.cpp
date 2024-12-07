#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_model.h"
#include "krado/io.h"
#include "builder.h"

using namespace krado;

TEST(WriteExodusIITest, line)
{
    auto shape = GeomShape(testing::build_line(Point(0, 0, 0), Point(1, 0, 0)));
    GeomModel model(shape);
    MeshModel mesh(model);

    auto & line = mesh.curve(1);
    // clang-format off
    line.set_scheme("bias")
        .set("intervals", 5)
        .set("coef", 1.2);
    // clang-format on
    mesh.mesh_curve(1);

    mesh.number_points();
    mesh.build_elements();

    // IO::export_mesh("line.exo", mesh);
}
