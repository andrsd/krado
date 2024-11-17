#include "gmock/gmock.h"
#include "krado/element.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh.h"
#include "krado/exodusii_file.h"
#include "builder.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(MeshTest, ctor)
{
    auto shape = GeomShape(testing::build_box(Point(0, 0, 0), Point(1, 1, 1)));
    GeomModel model(shape);
    Mesh mesh(model);
    EXPECT_EQ(mesh.vertices().size(), 8);
    EXPECT_EQ(mesh.curves().size(), 12);
    EXPECT_EQ(mesh.surfaces().size(), 6);
    EXPECT_EQ(mesh.volumes().size(), 1);

    auto box = mesh.volume(1);
    EXPECT_EQ(box.surfaces().size(), 6);
}

TEST(MeshTest, circle)
{
    auto shape = GeomShape(testing::build_circle(Point(0, 0, 0), 2.));
    GeomModel model(shape);
    Mesh mesh(model);
    EXPECT_EQ(mesh.vertices().size(), 1);
    EXPECT_EQ(mesh.curves().size(), 1);
    EXPECT_EQ(mesh.surfaces().size(), 1);
}

TEST(MeshTest, scaled)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read().scaled(0.25);
    auto & pnts = mesh.points();
    EXPECT_EQ(pnts.size(), 4);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(0.5, 0));
    EXPECT_EQ(pnts[2], Point(0, 0.5));
    EXPECT_EQ(pnts[3], Point(0.5, 0.5));
}

TEST(MeshTest, translated)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read().translated(2, 3);
    auto & pnts = mesh.points();
    EXPECT_EQ(pnts.size(), 4);
    EXPECT_EQ(pnts[0], Point(2, 3));
    EXPECT_EQ(pnts[1], Point(4, 3));
    EXPECT_EQ(pnts[2], Point(2, 5));
    EXPECT_EQ(pnts[3], Point(4, 5));
}

TEST(MeshTest, add_mesh)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto square = f.read();

    Mesh m;
    m.add(square);
    auto sq2 = square.translated(2, 0);
    m.add(sq2);

    auto & pnts = m.points();
    EXPECT_EQ(pnts.size(), 8);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(2, 0));
    EXPECT_EQ(pnts[2], Point(0, 2));
    EXPECT_EQ(pnts[3], Point(2, 2));
    EXPECT_EQ(pnts[4], Point(2, 0));
    EXPECT_EQ(pnts[5], Point(4, 0));
    EXPECT_EQ(pnts[6], Point(2, 2));
    EXPECT_EQ(pnts[7], Point(4, 2));

    auto & elems = m.elements();
    EXPECT_EQ(elems.size(), 4);
    EXPECT_EQ(elems[0].type(), Element::TRI3);
    EXPECT_THAT(elems[0].ids(), ElementsAre(0, 1, 2));
    EXPECT_EQ(elems[1].type(), Element::TRI3);
    EXPECT_THAT(elems[1].ids(), ElementsAre(2, 1, 3));
    EXPECT_EQ(elems[2].type(), Element::TRI3);
    EXPECT_THAT(elems[2].ids(), ElementsAre(4, 5, 6));
    EXPECT_EQ(elems[3].type(), Element::TRI3);
    EXPECT_THAT(elems[3].ids(), ElementsAre(6, 5, 7));
}
