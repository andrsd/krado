#include "gmock/gmock.h"
#include "krado/io/step_file.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(IO_STEPFileTest, load)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";

    EXPECT_NO_THROW({
        STEPFile file(input_file.string());
        auto shape = file.load();
    });
}
