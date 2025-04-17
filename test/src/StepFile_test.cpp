#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "krado/step_file.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(STEPFileTest, load)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";

    EXPECT_NO_THROW({
        STEPFile file(input_file.string());
        auto shapes = file.load();
    });
}

TEST(STEPFileTest, load_non_existing)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "non-existent";

    EXPECT_THAT_THROW_MSG(
        {
            STEPFile file(input_file.string());
            auto shapes = file.load();
        },
        HasSubstr("Failed to load STEP file"));
}

TEST(STEPFileTest, load_materials)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "box-w-mat.step";

    EXPECT_NO_THROW({
        STEPFile file(input_file.string());
        auto shapes = file.load();
        ASSERT_EQ(shapes.size(), 1);
        auto box = shapes[0];
        EXPECT_EQ(box.material(), "steel");
        EXPECT_EQ(box.density(), 8.);
    });
}
