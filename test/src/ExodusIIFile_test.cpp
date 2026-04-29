#include "gmock/gmock.h"
#include "builder.h"
#include "krado/exodusii_file.h"
#include "krado/element.h"
#include "krado/mesh.h"
#include "krado/point.h"
#include "krado/box.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/line.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/structured.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(ExodusIIFileTest, write_geom_model_1d)
{
    auto line = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(line);

    SchemeEqual::Options opts;
    opts.intervals = 5;

    auto c = model.curve(1);
    c->set_scheme<SchemeEqual>(opts);
    c->set_marker(10);
    model.set_block_name(10, "line");

    model.vertex(1)->set_marker(100);
    model.set_side_set_name(100, "left");
    model.vertex(2)->set_marker(101);
    model.set_side_set_name(101, "right");

    model.mesh_curve(1);

    ExodusIIFile f("line-1d.exo");
    f.write(model);

    EXPECT_TRUE(fs::exists("line-1d.exo"));
    fs::remove("line-1d.exo");
}

TEST(ExodusIIFileTest, write_geom_model_2d)
{
    Point pt1(0, 0, 0);
    Point pt2(3, 2, 0);
    auto rect = testing::build_rect(pt1, pt2);
    GeomModel model(rect);

    SchemeEqual::Options opts_h;
    opts_h.intervals = 3;
    SchemeEqual::Options opts_v;
    opts_v.intervals = 2;

    model.curve(1)->set_scheme<SchemeEqual>(opts_h);
    model.curve(1)->set_marker(1000);
    model.curve(2)->set_scheme<SchemeEqual>(opts_v);
    model.curve(2)->set_marker(1001);
    model.curve(3)->set_scheme<SchemeEqual>(opts_h);
    model.curve(3)->set_marker(1002);
    model.curve(4)->set_scheme<SchemeEqual>(opts_v);
    model.curve(4)->set_marker(1003);

    auto surf = model.surface(1);
    SchemeStructured::Options opts;
    surf->set_scheme<SchemeStructured>(opts);
    surf->set_marker(100);

    model.mesh_surface(1);

    model.set_block_name(100, "rectangle");
    model.set_side_set_name(1000, "bottom");
    model.set_side_set_name(1001, "right");
    model.set_side_set_name(1002, "top");
    model.set_side_set_name(1003, "left");

    ExodusIIFile f("rect-2d.exo");
    f.write(model);

    EXPECT_TRUE(fs::exists("rect-2d.exo"));
    fs::remove("rect-2d.exo");
}

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
    EXPECT_THAT(elems[0].type(), Eq(ElementType::TRI3));
    EXPECT_THAT(elems[0].ids(), ElementsAre(0, 1, 2));
    EXPECT_THAT(elems[1].type(), Eq(ElementType::TRI3));
    EXPECT_THAT(elems[1].ids(), ElementsAre(2, 1, 3));

    auto cell_set_ids = mesh.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = mesh.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

#if 0
    auto side_set_ids = mesh.side_set_ids();
    EXPECT_THAT(side_set_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
#endif
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
    mesh.set_face_set(100, edges_left);
    mesh.set_face_set_name(100, "left");
    std::vector<std::size_t> edges_right = { 9 };
    mesh.set_face_set(101, edges_right);

    ExodusIIFile f("sq-2d.exo");
    f.write(mesh);
}

TEST(ExodusIIFileTest, write_mesh_with_side_sets)
{
    std::vector<Point> pts = { Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0), Point(0, 1, 0) };
    std::vector<Element> elems = { Element::Quad4({ 0, 1, 2, 3 }) };
    Mesh mesh(pts, elems);
    mesh.set_up();
    auto bnd_edges = mesh.boundary_edges();
    mesh.set_edge_set(10, { bnd_edges[0] });
    mesh.set_edge_set_name(10, "bottom");

    {
        ExodusIIFile f("quad-2d.exo");
        f.write(mesh);
    }
    {
        ExodusIIFile f_read("quad-2d.exo");
        auto mesh_read = f_read.read();
        auto side_set_ids = mesh_read.edge_set_ids();
        EXPECT_THAT(side_set_ids, ElementsAre(10));
        auto & ss10 = mesh_read.edge_set(10);
        EXPECT_EQ(ss10.size(), 1);
    }
}
