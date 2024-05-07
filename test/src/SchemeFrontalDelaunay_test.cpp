#include "gmock/gmock.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(SchemeFrontalDelaunayTest, mesh_quarter_circle)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    GeomModel model(shape);

    // clang-format off
    model.curve(1)
        .set("marker", 101)
        .set_scheme("equal")
            .set("intervals", 4);
    // clang-format on
    model.mesh_curve(1);

    // clang-format off
    model.curve(2)
        .set("marker", 101);
    // clang-format on

    // clang-format off
    model.curve(3)
        .set("marker", 101);
    // clang-format on

    // clang-format off
    model.surface(1)
        .set("marker", 10)
        .set_scheme("frontal-delaunay")
            .set("max_area", 0.25);
    // clang-format on
    model.mesh_surface(1);

    auto & srf = model.surface(1);

    EXPECT_EQ(srf.all_vertices().size(), 7);
    EXPECT_EQ(srf.surface_vertices().size(), 1);

    EXPECT_EQ(srf.elements().size(), 6);
}
