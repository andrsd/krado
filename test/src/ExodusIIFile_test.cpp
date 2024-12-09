#include "gmock/gmock.h"
#include "krado/exodusii_file.h"
#include "krado/element.h"
#include "krado/mesh.h"
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

    auto cell_set_ids = mesh.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = mesh.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

    auto side_set_ids = mesh.side_set_ids();
    EXPECT_THAT(side_set_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
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
        Element::Tri3({ 0, 1, 2 }),
        Element::Tri3({ 2, 1, 3 })
    };
    // clang-format on

    Mesh mesh(pts, elems);
    mesh.set_up();
    mesh.set_cell_set(1, { 0 });
    mesh.set_cell_set(2, { 1 });
    std::vector<std::size_t> edges_left = { 8 };
    mesh.set_side_set(100, edges_left);
    mesh.set_side_set_name(100, "left");
    std::vector<std::size_t> edges_right = { 9 };
    mesh.set_side_set(101, edges_right);

    ExodusIIFile f("sq-2d.exo");
    f.write(mesh);
}
