#include "gmock/gmock.h"
#include "krado/exodusii_file.h"
#include "krado/element.h"
#include "krado/point.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(ExodusIIFileTest, read_2d)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read();

    auto & pnts = mesh.points();
    EXPECT_EQ(pnts.size(), 4);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(2, 0));
    EXPECT_EQ(pnts[2], Point(0, 2));
    EXPECT_EQ(pnts[3], Point(2, 2));

    auto & elems = mesh.elements();
    EXPECT_EQ(elems.size(), 2);
    EXPECT_THAT(elems[0].type(), Eq(Element::TRI3));
    EXPECT_THAT(elems[0].ids(), ElementsAre(0, 1, 2));
    EXPECT_THAT(elems[1].type(), Eq(Element::TRI3));
    EXPECT_THAT(elems[1].ids(), ElementsAre(2, 1, 3));

    auto side_set_ids = mesh.side_set_ids();
    EXPECT_THAT(side_set_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0].elem, 1);
    EXPECT_EQ(ss0[0].side, 1);

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0].elem, 0);
    EXPECT_EQ(ss1[0].side, 2);
}

TEST(ExodusIIFileTest, write_2d)
{
    // clang-format off
    std::vector<Point> pts = {
        Point(0., 0.),
        Point(1., 0.),
        Point(0., 1.),
        Point(1., 1.)
    };
    std::vector<Element> elems = {
        Element::Tri3({ 0, 1, 2 }, 1),
        Element::Tri3({ 2, 1, 3 }, 2)
    };
    // clang-format on

    Mesh mesh(pts, elems);
    mesh.set_up();
    std::vector<std::size_t> edges_left = { 8 };
    mesh.set_side_set(100, edges_left);
    mesh.set_side_set_name(100, "left");
    std::vector<std::size_t> edges_right = { 9 };
    mesh.set_side_set(101, edges_right);

    ExodusIIFile f("sq-2d.exo");
    f.write(mesh);
}
