#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "krado/io/step_file.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(IO_STEPFileTest, load)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";

    EXPECT_NO_THROW({
        STEPFile file(input_file.string());
        auto shape = file.load();
    });
}

TEST(IO_STEPFileTest, load_non_existing)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "non-existent";

    EXPECT_THAT_THROW_MSG({
        STEPFile file(input_file.string());
        auto shape = file.load();
    }, HasSubstr("Could not read file"));
}
