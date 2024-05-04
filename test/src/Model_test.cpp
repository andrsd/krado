#include "gmock/gmock.h"
#include "krado/io/step_file.h"
#include "krado/geo/model.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(ModelTest, load)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";

    STEPFile file(input_file.string());
    auto shape = file.load();
    geo::Model model(shape);
}
