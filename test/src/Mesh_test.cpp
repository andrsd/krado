#include "gmock/gmock.h"
#include "krado/element.h"
#include "krado/mesh.h"
#include "krado/exodusii_file.h"
#include "krado/range.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(MeshTest, point)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read();
    EXPECT_THAT(mesh.point(0), Point(0, 0));
    EXPECT_THAT(mesh.point(1), Point(2, 0));
    EXPECT_THAT(mesh.point(2), Point(0, 2));
    EXPECT_THAT(mesh.point(3), Point(2, 2));
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

    auto cell_set_ids = mesh.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = mesh.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

    auto ss_ids = mesh.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
}

TEST(MeshTest, scale)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read();
    mesh.scale(0.25);
    auto & pnts = mesh.points();
    EXPECT_EQ(pnts.size(), 4);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(0.5, 0));
    EXPECT_EQ(pnts[2], Point(0, 0.5));
    EXPECT_EQ(pnts[3], Point(0.5, 0.5));

    auto cell_set_ids = mesh.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = mesh.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

    auto ss_ids = mesh.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
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

    auto cell_set_ids = mesh.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = mesh.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

    auto ss_ids = mesh.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
}

TEST(MeshTest, translate)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read();
    mesh.translate(2, 3);
    auto & pnts = mesh.points();
    EXPECT_EQ(pnts.size(), 4);
    EXPECT_EQ(pnts[0], Point(2, 3));
    EXPECT_EQ(pnts[1], Point(4, 3));
    EXPECT_EQ(pnts[2], Point(2, 5));
    EXPECT_EQ(pnts[3], Point(4, 5));

    auto cell_set_ids = mesh.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = mesh.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

    auto ss_ids = mesh.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = mesh.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = mesh.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
}

TEST(MeshTest, add_mesh)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto square = f.read();

    Mesh m;
    m.add(square);
    auto sq2 = square.translated(0, 2);
    m.add(sq2);

    auto & pnts = m.points();
    EXPECT_EQ(pnts.size(), 8);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(2, 0));
    EXPECT_EQ(pnts[2], Point(0, 2));
    EXPECT_EQ(pnts[3], Point(2, 2));
    EXPECT_EQ(pnts[4], Point(0, 2));
    EXPECT_EQ(pnts[5], Point(2, 2));
    EXPECT_EQ(pnts[6], Point(0, 4));
    EXPECT_EQ(pnts[7], Point(2, 4));

    auto & elems = m.elements();
    EXPECT_EQ(elems.size(), 4);
    EXPECT_EQ(elems[0].type(), ElementType::TRI3);
    EXPECT_THAT(elems[0].ids(), ElementsAre(0, 1, 2));
    EXPECT_EQ(elems[1].type(), ElementType::TRI3);
    EXPECT_THAT(elems[1].ids(), ElementsAre(2, 1, 3));
    EXPECT_EQ(elems[2].type(), ElementType::TRI3);
    EXPECT_THAT(elems[2].ids(), ElementsAre(4, 5, 6));
    EXPECT_EQ(elems[3].type(), ElementType::TRI3);
    EXPECT_THAT(elems[3].ids(), ElementsAre(6, 5, 7));

    auto cell_set_ids = m.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = m.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1, 2, 3));

    auto ss_ids = m.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = m.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));
    EXPECT_EQ(ss0[1], side_set_entry_t(3, 1));

    auto & ss1 = m.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
    EXPECT_EQ(ss1[1], side_set_entry_t(2, 2));
}

TEST(MeshTest, remove_duplicate_points)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto square = f.read();

    Mesh m;
    m.add(square);
    auto sq2 = square.translated(2, 0);
    m.add(sq2);

    m.remove_duplicate_points();
    auto & pnts = m.points();

    EXPECT_EQ(pnts.size(), 6);
    EXPECT_EQ(pnts[0], Point(0, 0));
    EXPECT_EQ(pnts[1], Point(2, 0));
    EXPECT_EQ(pnts[2], Point(0, 2));
    EXPECT_EQ(pnts[3], Point(2, 2));
    EXPECT_EQ(pnts[4], Point(4, 0));
    EXPECT_EQ(pnts[5], Point(4, 2));

    auto & elems = m.elements();
    EXPECT_EQ(elems.size(), 4);
    EXPECT_EQ(elems[0].type(), ElementType::TRI3);
    EXPECT_THAT(elems[0].ids(), ElementsAre(0, 1, 2));
    EXPECT_EQ(elems[1].type(), ElementType::TRI3);
    EXPECT_THAT(elems[1].ids(), ElementsAre(2, 1, 3));
    EXPECT_EQ(elems[2].type(), ElementType::TRI3);
    EXPECT_THAT(elems[2].ids(), ElementsAre(1, 4, 3));
    EXPECT_EQ(elems[3].type(), ElementType::TRI3);
    EXPECT_THAT(elems[3].ids(), ElementsAre(3, 4, 5));

    auto bbox = m.compute_bounding_box();
    EXPECT_NEAR(bbox.size(0), 4., 1e-15);
    EXPECT_NEAR(bbox.size(1), 2., 1e-15);
}

TEST(MeshTest, duplicate)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto square = f.read();

    auto dup = square.duplicate();
    EXPECT_THAT(square.points(), Eq(dup.points()));
    EXPECT_THAT(square.elements(), Eq(dup.elements()));

    auto cell_set_ids = dup.cell_set_ids();
    EXPECT_THAT(cell_set_ids, ElementsAre(0));

    auto & cs0 = dup.cell_set(0);
    EXPECT_THAT(cs0, ElementsAre(0, 1));

    auto ss_ids = dup.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = dup.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(1, 1));

    auto & ss1 = dup.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(0, 2));
}

TEST(MeshTest, remap_block_ids)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto square = f.read();
    square.set_cell_set(0, { 0, 1 });
    square.remap_block_ids({ { 0, 1000 } });

    EXPECT_THAT(square.cell_set_ids(), ElementsAre(1000));
    EXPECT_THAT(square.cell_set(1000), ElementsAre(0, 1));
}

TEST(MeshTest, element_ids_2d)
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
    EXPECT_EQ(mesh.cell_ids(), krado::Range(0, 1));
    EXPECT_EQ(mesh.edge_ids(), krado::Range(6, 10));
    EXPECT_EQ(mesh.point_ids(), krado::Range(2, 5));
}

TEST(MeshTest, element_ids_3d)
{
    // clang-format off
    std::vector<Point> pts = {
        Point(0., 0., 0.),
        Point(1., 0., 0.),
        Point(1., 1., 0.),
        Point(0., 1., 0.),
        Point(0., 0., 1.),
        Point(1., 0., 1.),
        Point(1., 1., 1.),
        Point(0., 1., 1.)
    };
    std::vector<Element> elems = {
        Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 })
    };
    // clang-format on

    Mesh mesh(pts, elems);
    mesh.set_up();
    EXPECT_EQ(mesh.cell_ids(), krado::Range(0, 0));
    EXPECT_EQ(mesh.face_ids(), krado::Range(9, 14));
    EXPECT_EQ(mesh.edge_ids(), krado::Range(15, 26));
    EXPECT_EQ(mesh.point_ids(), krado::Range(1, 8));
}

TEST(MeshTest, element_ids_from_file_2d)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto m = f.read();
    m.set_up();

    EXPECT_EQ(m.cell_ids(), krado::Range(0, 1));
    EXPECT_EQ(m.edge_ids(), krado::Range(6, 10));
    // EXPECT_EQ(m.face_ids(), krado::Range());
    EXPECT_EQ(m.point_ids(), krado::Range(2, 5));

    EXPECT_THAT(m.support(0), ElementsAre());
    EXPECT_THAT(m.support(1), ElementsAre());
    EXPECT_THAT(m.support(2), ElementsAre(6, 8));
    EXPECT_THAT(m.support(3), ElementsAre(6, 7, 9));
    EXPECT_THAT(m.support(4), ElementsAre(7, 8, 10));
    EXPECT_THAT(m.support(5), ElementsAre(9, 10));
    EXPECT_THAT(m.support(6), ElementsAre(0));
    EXPECT_THAT(m.support(7), ElementsAre(0, 1));
    EXPECT_THAT(m.support(8), ElementsAre(0));
    EXPECT_THAT(m.support(9), ElementsAre(1));
    EXPECT_THAT(m.support(10), ElementsAre(1));

    EXPECT_THAT(m.cone(0), ElementsAre(6, 7, 8));
    EXPECT_THAT(m.cone(1), ElementsAre(7, 9, 10));
    EXPECT_THAT(m.cone(2), ElementsAre());
    EXPECT_THAT(m.cone(3), ElementsAre());
    EXPECT_THAT(m.cone(4), ElementsAre());
    EXPECT_THAT(m.cone(5), ElementsAre());
    EXPECT_THAT(m.cone(6), ElementsAre(2, 3));
    EXPECT_THAT(m.cone(7), ElementsAre(3, 4));
    EXPECT_THAT(m.cone(8), ElementsAre(4, 2));
    EXPECT_THAT(m.cone(9), ElementsAre(3, 5));
    EXPECT_THAT(m.cone(10), ElementsAre(5, 4));
}

TEST(MeshTest, element_ids_from_file_3d)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "cube-tet.e");
    auto m = f.read();
    m.set_up();

    EXPECT_EQ(m.cell_ids(), krado::Range(0, 5));
    EXPECT_EQ(m.face_ids(), krado::Range(14, 31));
    EXPECT_EQ(m.edge_ids(), krado::Range(32, 50));
    EXPECT_EQ(m.point_ids(), krado::Range(6, 13));

    ExodusIIFile out("a.e");
    out.write(m);
}

TEST(MeshTest, boundary_edges)
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

    auto bnd_edges = mesh.boundary_edges();
    EXPECT_THAT(bnd_edges, UnorderedElementsAre(6, 8, 9, 10));
}

TEST(MeshTest, boundary_faces)
{
    // clang-format off
    std::vector<Point> pts = {
        Point(0., 0., 0.),
        Point(1., 0., 0.),
        Point(1., 1., 0.),
        Point(0., 1., 0.),
        Point(0., 0., 1.),
        Point(1., 0., 1.),
        Point(1., 1., 1.),
        Point(0., 1., 1.)
    };
    std::vector<Element> elems = {
        Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 })
    };
    // clang-format on

    Mesh mesh(pts, elems);
    mesh.set_up();

    auto bnd_faces = mesh.boundary_faces();
    EXPECT_THAT(bnd_faces, UnorderedElementsAre(9, 10, 11, 12, 13, 14));
}

TEST(MeshTest, centroid_2d)
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

    EXPECT_EQ(mesh.compute_centroid(0), Point(1. / 3., 1. / 3., 0));
    EXPECT_EQ(mesh.compute_centroid(1), Point(2. / 3., 2. / 3., 0));
    EXPECT_EQ(mesh.compute_centroid(6), Point(0.5, 0., 0));
    EXPECT_EQ(mesh.compute_centroid(7), Point(0.5, 0.5, 0));
    EXPECT_EQ(mesh.compute_centroid(8), Point(0., 0.5, 0));
    EXPECT_EQ(mesh.compute_centroid(9), Point(1., 0.5, 0));
    EXPECT_EQ(mesh.compute_centroid(10), Point(0.5, 1., 0));
}

TEST(MeshTest, centroid_3d)
{
    // clang-format off
    std::vector<Point> pts = {
        Point(0., 0., 0.),
        Point(1., 0., 0.),
        Point(1., 1., 0.),
        Point(0., 1., 0.),
        Point(0., 0., 1.),
        Point(1., 0., 1.),
        Point(1., 1., 1.),
        Point(0., 1., 1.)
    };
    std::vector<Element> elems = {
        Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 })
    };
    // clang-format on

    Mesh mesh(pts, elems);
    mesh.set_up();

    EXPECT_EQ(mesh.compute_centroid(0), Point(0.5, 0.5, 0.5));
    EXPECT_EQ(mesh.compute_centroid(9), Point(0.5, 0., 0.5));
    EXPECT_EQ(mesh.compute_centroid(10), Point(0.5, 1., 0.5));
    EXPECT_EQ(mesh.compute_centroid(11), Point(0., 0.5, 0.5));
    EXPECT_EQ(mesh.compute_centroid(12), Point(1., 0.5, 0.5));
    EXPECT_EQ(mesh.compute_centroid(13), Point(0.5, 0.5, 0.));
    EXPECT_EQ(mesh.compute_centroid(14), Point(0.5, 0.5, 1.));
}

TEST(MeshTest, outward_normal_2d)
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

    EXPECT_EQ(mesh.outward_normal(6), Vector(0, -1, 0));
    EXPECT_EQ(mesh.outward_normal(9), Vector(1, 0, 0));
    EXPECT_EQ(mesh.outward_normal(10), Vector(0, 1, 0));
    EXPECT_EQ(mesh.outward_normal(8), Vector(-1, 0, 0));
}

TEST(MeshTest, outward_normal_3d)
{
    // clang-format off
    std::vector<Point> pts = {
        Point(0., 0., 0.),
        Point(1., 0., 0.),
        Point(1., 1., 0.),
        Point(0., 1., 0.),
        Point(0., 0., 1.),
        Point(1., 0., 1.),
        Point(1., 1., 1.),
        Point(0., 1., 1.)
    };
    std::vector<Element> elems = {
        Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 })
    };
    // clang-format on

    Mesh mesh(pts, elems);
    mesh.set_up();

    EXPECT_EQ(mesh.outward_normal(9), Vector(0, -1, 0));
    EXPECT_EQ(mesh.outward_normal(10), Vector(0, 1, 0));
    EXPECT_EQ(mesh.outward_normal(11), Vector(-1, 0, 0));
    EXPECT_EQ(mesh.outward_normal(12), Vector(1, 0, 0));
    EXPECT_EQ(mesh.outward_normal(13), Vector(0, 0, -1));
    EXPECT_EQ(mesh.outward_normal(14), Vector(0, 0, 1));
}
