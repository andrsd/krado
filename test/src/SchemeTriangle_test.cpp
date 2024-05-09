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

    auto & arc = mesh.curve(4);
    auto & mpars_arc = arc.meshing_parameters();
    mpars_arc.set<std::string>("scheme") = "equal";
    mpars_arc.set<int>("intervals") = 4;
    mpars_arc.set<int>("marker") = 101;
    mesh.mesh_curve(4);

    auto & side1 = mesh.curve(5);
    auto & mpars_side1 = side1.meshing_parameters();
    mpars_side1.set<int>("marker") = 102;

    auto & qcirc = mesh.surface(7);
    auto & mpars_qcirc = qcirc.meshing_parameters();
    mpars_qcirc.set<std::string>("scheme") = "triangle";
    mpars_qcirc.set<int>("marker") = 10;
    mpars_qcirc.set<double>("max_area") = 0.5;
    mpars_qcirc.set<std::tuple<double, double>>("region_point") = { 0.1, 0.1 };
    mesh.mesh_surface(7);

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
    Model model(shape);
    Mesh mesh(model);

    auto & qcirc = mesh.surface(7);
    auto & mpars_qcirc = qcirc.meshing_parameters();
    mpars_qcirc.set<std::string>("scheme") = "triangle";
    EXPECT_THROW_MSG({ mesh.mesh_surface(7); }, "krado was not built with triangle support.");
}

#endif
