#include "gmock/gmock.h"
#include "builder.h"
#include "krado/iges_file.h"
#include "krado/exception.h"
#include "krado/box.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(IGESFileTest, read_nonexistent_file)
{
    fs::path file = fs::path(KRADO_UNIT_TESTS_ROOT) / "no-such-file.iges";
    IGESFile step(file.string());
    EXPECT_THROW({ step.read(); }, Exception);
}

TEST(IGESFileTest, read)
{
    fs::path file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "geo" / "box.iges";
    IGESFile step(file.string());
    EXPECT_NO_THROW({ auto shape = step.read(); });
}

TEST(IGESFileTest, write)
{
    fs::path file = fs::path(fs::temp_directory_path()) / "krado-write.iges";
    IGESFile step(file.string());
    auto box = testing::build_box(Point(0, 0, 0), Point(1, 1, 1));
    box.set_name("box");
    box.set_material("steel", "SS316", 8);
    EXPECT_NO_THROW({ step.write({ box }); });
}
