#include "gmock/gmock.h"
#include "krado/step_file.h"
#include "krado/model.h"
#include "krado/exception.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(ModelTest, load)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";

    STEPFile file(input_file.string());
    auto shape = file.load();
    Model model(shape);

    auto v1 = model.vertex(1);
    EXPECT_DOUBLE_EQ(v1.x(), 0.);
    EXPECT_DOUBLE_EQ(v1.y(), 0.);
    EXPECT_DOUBLE_EQ(v1.z(), 0.);

    auto v2 = model.vertex(2);
    EXPECT_DOUBLE_EQ(v2.x(), 1.);
    EXPECT_DOUBLE_EQ(v2.y(), 0.);
    EXPECT_DOUBLE_EQ(v2.z(), 0.);

    auto curve = model.curve(3);
    EXPECT_EQ(model.vertex_id(curve.first_vertex()), 1);
    EXPECT_EQ(model.vertex_id(curve.last_vertex()), 2);
    EXPECT_EQ(model.curve_id(curve), 3);

    EXPECT_THROW({ model.vertex(0); }, Exception);
}

TEST(ModelTest, load_quad)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quad.step";

    STEPFile file(input_file.string());
    auto shape = file.load();
    Model model(shape);
    EXPECT_EQ(model.vertices().size(), 4);
    EXPECT_EQ(model.curves().size(), 4);

    EXPECT_EQ(model.surfaces().size(), 1);
    auto surface = model.surface(9);
    EXPECT_EQ(model.surface_id(surface), 9);
    EXPECT_THROW({ model.surface(1000); }, Exception);
}

TEST(ModelTest, load_box)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "box.step";

    STEPFile file(input_file.string());
    auto shape = file.load();
    Model model(shape);
    EXPECT_EQ(model.vertices().size(), 8);
    EXPECT_EQ(model.curves().size(), 12);
    EXPECT_EQ(model.surfaces().size(), 6);

    EXPECT_EQ(model.volumes().size(), 1);
    auto box = model.volume(27);
    EXPECT_EQ(model.volume_id(box), 27);
    EXPECT_THROW({ model.volume(1000); }, Exception);
}
