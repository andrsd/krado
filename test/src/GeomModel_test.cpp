#include "gmock/gmock.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/exception.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(GeomModelTest, load)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";

    STEPFile file(input_file.string());
    auto shapes = file.load();
    GeomModel model(shapes[0]);

    auto v1 = model.vertex(1);
    EXPECT_EQ(v1.point(), Point(0., 0., 0.));

    auto v2 = model.vertex(2);
    EXPECT_EQ(v2.point(), Point(1., 0., 0.));

    EXPECT_THROW({ auto & v = model.vertex(0); }, Exception);
}

TEST(GeomModelTest, load_quad)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quad.step";

    STEPFile file(input_file.string());
    auto shapes = file.load();
    GeomModel model(shapes[0]);
    EXPECT_EQ(model.vertices().size(), 4);
    EXPECT_EQ(model.curves().size(), 4);

    EXPECT_EQ(model.surfaces().size(), 1);
    EXPECT_THROW({ auto & s = model.surface(1000); }, Exception);
}

TEST(GeomModelTest, load_box)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "box.step";

    STEPFile file(input_file.string());
    auto shapes = file.load();
    GeomModel model(shapes[0]);
    EXPECT_EQ(model.vertices().size(), 8);
    EXPECT_EQ(model.curves().size(), 12);
    EXPECT_EQ(model.surfaces().size(), 6);

    EXPECT_EQ(model.volumes().size(), 1);
    EXPECT_THROW({ auto & v = model.volume(1000); }, Exception);
}
