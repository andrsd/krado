#include "gmock/gmock.h"
#include "krado/heal.h"
#include "krado/step_file.h"

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(OperationsTest, heal)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "geo" / "line.step";
    STEPFile file(input_file.string());
    auto shapes = file.read();
    auto shape = heal(shapes[0], 1e-10);
    // TODO: actual check that things were healed
}
