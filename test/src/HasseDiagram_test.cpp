#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/mesh.h"
#include "krado/hasse_diagram.h"

using namespace krado;

TEST(HasseDiagramTest, hasse_1d)
{
    // clang-format off
    std::vector<Point> pts = {
        Point(0.),
        Point(1.),
        Point(2.),
    };
    std::vector<Element> elems = {
        Element::Line2({ 0, 1 }),
        Element::Line2({ 1, 2 }),
    };
    // clang-format on
    Mesh mesh(pts, elems);
    HasseDiagram hasse(mesh);
}

TEST(HasseDiagramTest, hasse_2d)
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
    HasseDiagram hasse(mesh);
}

TEST(HasseDiagramTest, hasse_3d)
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
    HasseDiagram hasse(mesh);
}
