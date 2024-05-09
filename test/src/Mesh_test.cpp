#include "gmock/gmock.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(MeshTest, ctor)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "box.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    GeomModel model(shape);
    Mesh mesh(model);
    EXPECT_EQ(mesh.vertices().size(), 8);
    EXPECT_EQ(mesh.curves().size(), 12);
    EXPECT_EQ(mesh.surfaces().size(), 6);
    EXPECT_EQ(mesh.volumes().size(), 1);

    auto box = mesh.volume(27);
    EXPECT_EQ(box.surfaces().size(), 6);
}
