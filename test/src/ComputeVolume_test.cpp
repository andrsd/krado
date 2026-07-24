#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex.h"
#include "krado/ops.h"
#include "krado/exodusii_file.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(ComputeVolumeTest, length_of_a_line)
{
    std::vector<Point> points = {
        Point(0, 0, 0),      Point(0.75, 1.0, 0), Point(1.5, 2.0, 0),
        Point(2.25, 3.0, 0), Point(3.0, 4.0, 0),
    };
    std::vector<Element> elements = {
        Element::Line2({ 0, 1 }),
        Element::Line2({ 1, 2 }),
        Element::Line2({ 2, 3 }),
        Element::Line2({ 3, 4 }),
    };
    Mesh mesh(points, elements);
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(5., 1e-10))));
}

TEST(ComputeVolumeTest, area_of_a_square)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "mesh" / "square-half-tri.e");
    auto mesh = f.read();
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(4., 1e-10))));
}

TEST(ComputeVolumeTest, volume_of_a_cube_tet4)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "mesh" / "cube-tet.e");
    auto mesh = f.read();
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(1., 1e-10))));
}

TEST(ComputeVolumeTest, area_of_a_quad4)
{
    std::vector<Point> points = {
        Point(0, 0, 0),
        Point(1, 0, 0),
        Point(1, 1, 0),
        Point(0, 1, 0),
    };
    std::vector<Element> elements = { Element::Quad4({ 0, 1, 2, 3 }) };
    Mesh mesh(points, elements);
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(1., 1e-10))));
}

TEST(ComputeVolumeTest, volume_of_a_hex8)
{
    std::vector<Point> points = {
        Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0), Point(0, 1, 0),
        Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1),
    };
    std::vector<Element> elements = { Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 }) };
    Mesh mesh(points, elements);
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(1., 1e-10))));
}

TEST(ComputeVolumeTest, volume_of_a_prism6)
{
    std::vector<Point> points = {
        Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0),
        Point(0, 0, 1), Point(1, 0, 1), Point(0, 1, 1),
    };
    std::vector<Element> elements = { Element::Prism6({ 0, 1, 2, 3, 4, 5 }) };
    Mesh mesh(points, elements);
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(0.5, 1e-10))));
}

TEST(ComputeVolumeTest, volume_of_a_pyramid5)
{
    std::vector<Point> points = {
        Point(-1, -1, 0), Point(1, -1, 0), Point(1, 1, 0), Point(-1, 1, 0), Point(0, 0, 1),
    };
    std::vector<Element> elements = { Element::Pyramid5({ 0, 1, 2, 3, 4 }) };
    Mesh mesh(points, elements);
    auto vols = compute_volume(mesh);
    // Base area = 4, height = 1, Volume = 4/3
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(4. / 3., 1e-10))));
}

TEST(ComputeVolumeTest, area_of_a_quad4_blocks)
{
    // Domain: 4x2
    std::vector<Point> pts = {
        Point(0, 0, 0), Point(2, 0, 0), Point(4, 0, 0), Point(0, 1, 0), Point(2, 1, 0),
        Point(4, 1, 0), Point(0, 2, 0), Point(2, 2, 0), Point(4, 2, 0),
    };
    std::vector<Element> elements = {
        Element::Quad4({ 0, 1, 4, 3 }), // Quad 0
        Element::Quad4({ 1, 2, 5, 4 }), // Quad 1
        Element::Quad4({ 3, 4, 7, 6 }), // Quad 2
        Element::Quad4({ 4, 5, 8, 7 }), // Quad 3
    };
    Mesh mesh(pts, elements);
    // Block 1: Quad 0 (area 2)
    mesh.set_cell_set(10, { 0 });
    // Block 2: Quads 1, 2, 3 (area 2+2+2 = 6)
    mesh.set_cell_set(20, { 1, 2, 3 });

    auto vols = compute_volume(mesh);
    EXPECT_THAT(
        vols,
        UnorderedElementsAre(Pair(10, DoubleNear(2., 1e-10)), Pair(20, DoubleNear(6., 1e-10))));
}

TEST(ComputeVolumeTest, area_of_a_quad4_skewed)
{
    std::vector<Point> points = {
        Point(0, 0, 0),
        Point(2, 0, 0),
        Point(3, 2, 0),
        Point(1, 2, 0),
    };
    std::vector<Element> elements = { Element::Quad4({ 0, 1, 2, 3 }) };
    Mesh mesh(points, elements);
    auto vols = compute_volume(mesh);
    // Parallelogram with base 2 and height 2. Area = 4.
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(4., 1e-10))));
}
