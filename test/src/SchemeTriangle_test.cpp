#include "gmock/gmock.h"
#include "krado/config.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
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
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    // clang-format off
    model.curve(1)
         .set("marker", 101)
         .set_scheme("equal")
            .set("intervals", 4);
    model.mesh_curve(1);

    model.curve(2)
         .set("marker", 102);

    model.surface(1)
         .set("marker", 10)
         .set_scheme("triangle")
             .set("max_area", 0.5)
             .set<std::tuple<double, double>>("region_point", { 0.1, 0.1 });
    model.mesh_surface(1);
    // clang-format on

    auto & qcirc = model.surface(1);
    EXPECT_EQ(qcirc.all_vertices().size(), 6);
    EXPECT_EQ(qcirc.triangles().size(), 4);

    auto mesh = model.build_mesh();
    auto & pts = mesh.points();
    ASSERT_EQ(pts.size(), 6);

    auto elems = mesh.elements();
    ASSERT_EQ(elems.size(), 4);
}

#else

TEST(SchemeTriangleTest, mesh)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    auto qcirc = model.surface(1);
    qcirc->set_scheme("triangle");
    EXPECT_THROW_MSG({ model.mesh_surface(1); }, "krado was not built with triangle support.");
}

#endif
