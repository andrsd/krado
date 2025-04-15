#include "gmock/gmock.h"
#include "builder.h"
#include "krado/step_file.h"
#include "krado/geom_shape.h"
#include "krado/geom_vertex.h"
#include "BRepLib_MakeVertex.hxx"
#include "TopoDS.hxx"

using namespace krado;
namespace fs = std::filesystem;

TEST(GeomShapeTest, clean)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    shape.clean();
    // TODO: actual check that clean happened
}

TEST(GeomShapeTest, heal)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    shape.heal(1e-10);
    // TODO: actual check that things were healed
}

TEST(GeomShapeTest, scale)
{
    gp_Pnt pt(1, 2, 3);
    BRepLib_MakeVertex make_vtx(pt);
    make_vtx.Build();
    auto vtx = make_vtx.Vertex();

    GeomShape sh(1, vtx);
    sh.scale(0.5);

    GeomVertex v(TopoDS::Vertex(sh));
    EXPECT_DOUBLE_EQ(v.x(), 0.5);
    EXPECT_DOUBLE_EQ(v.y(), 1.);
    EXPECT_DOUBLE_EQ(v.z(), 1.5);
}

TEST(GeomShapeTest, material)
{
    auto box = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    EXPECT_FALSE(box.has_material());
    box.set_material("steel", 8);
    EXPECT_TRUE(box.has_material());
    EXPECT_EQ(box.material(), "steel");
    EXPECT_DOUBLE_EQ(box.density(), 8.);
}
