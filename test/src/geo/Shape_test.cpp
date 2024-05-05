#include "gmock/gmock.h"
#include "krado/io/step_file.h"
#include "krado/geo/shape.h"
#include "krado/geo/vertex.h"
#include "BRepLib_MakeVertex.hxx"
#include "TopoDS.hxx"

using namespace krado;
namespace fs = std::filesystem;

TEST(Geo_ShapeTest, clean)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    shape.clean();
    // TODO: actual check that clean happened
}

TEST(Geo_ShapeTest, heal)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "line.step";
    STEPFile file(input_file.string());
    auto shape = file.load();
    shape.heal(1e-10);
    // TODO: actual check that things were healed
}

TEST(Geo_ShapeTest, scale) {
    gp_Pnt pt(1, 2, 3);
    BRepLib_MakeVertex make_vtx(pt);
    make_vtx.Build();
    auto vtx = make_vtx.Vertex();

    geo::Shape sh(vtx);
    sh.scale(0.5);

    geo::Vertex v(TopoDS::Vertex(sh));
    EXPECT_DOUBLE_EQ(v.x(), 0.5);
    EXPECT_DOUBLE_EQ(v.y(), 1.);
    EXPECT_DOUBLE_EQ(v.z(), 1.5);
}
