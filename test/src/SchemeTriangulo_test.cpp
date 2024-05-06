#include "gmock/gmock.h"
#include "krado/step_file.h"
#include "krado/model.h"
#include "krado/mesh.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(SchemeTrianguloTest, mesh_quarter_circle)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    Model model(shape);
    Mesh mesh(model);

    auto & arc = mesh.curve(4);
    auto & mpars_arc = arc.meshing_parameters();
    mpars_arc.set<std::string>("scheme") = "equal";
    mpars_arc.set<int>("intervals") = 4;
    mpars_arc.set<int>("marker") = 101;
    mesh.mesh_curve(4);

    auto & side1 = mesh.curve(5);
    auto & mpars_side1 = arc.meshing_parameters();
    mpars_side1.set<int>("marker") = 101;

    auto & qcirc = mesh.surface(7);
    auto & mpars_qcirc = qcirc.meshing_parameters();
    mpars_qcirc.set<std::string>("scheme") = "triangulo";
    mpars_qcirc.set<int>("marker") = 10;
    mpars_qcirc.set<double>("max_area") = 0.25;
    mesh.mesh_surface(7);
}
