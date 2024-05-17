#include "gmock/gmock.h"
#include "krado/config.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include "ExceptionTestMacros.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

#ifdef KRADO_WITH_TRIANGLE

TEST(SchemeTriangleTest, mesh_quarter_circle)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    GeomModel model(shape);
    Mesh mesh(model);

    // clang-format off
    mesh.curve(1)
        .set("marker", 101)
        .set_scheme("equal")
            .set("intervals", 4)
            .set("marker", 101);
    mesh.mesh_curve(1);

    mesh.curve(2)
        .set("marker", 102);

    mesh.surface(1)
        .set("marker", 10)
        .set_scheme("triangle")
            .set("max_area", 0.5)
            .set<std::tuple<double, double>>("region_point", { 0.1, 0.1 });
    mesh.mesh_surface(1);
    // clang-format on

    auto & qcirc = mesh.surface(1);
    EXPECT_EQ(qcirc.all_vertices().size(), 6);
    EXPECT_EQ(qcirc.triangles().size(), 4);

    mesh.number_points();
    auto pts = mesh.points();
    ASSERT_EQ(pts.size(), 6);

    mesh.build_elements();
    auto elems = mesh.elements();
    ASSERT_EQ(elems.size(), 4);
}

#else

TEST(SchemeTriangleTest, mesh)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    GeomModel model(shape);
    Mesh mesh(model);

    auto & qcirc = mesh.surface(1);
    qcirc.set_scheme("triangle");
    EXPECT_THROW_MSG({ mesh.mesh_surface(1); }, "krado was not built with triangle support.");
}

#endif
