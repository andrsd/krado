#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/ops.h"
#include "krado/point.h"
#include "krado/box.h"
#include "krado/axis1.h"
#include "krado/axis2.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "krado/circle.h"
#include "krado/plane.h"
#include "krado/wire.h"
#include "krado/line.h"
#include "krado/arc_of_circle.h"

using namespace krado;
using namespace testing;

namespace {

double
radians(double deg)
{
    return deg * M_PI / 180.;
}

} // namespace

TEST(OperationsTest, translate_vec)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto res = translate(box, Vector(1, 0, 0));
}

TEST(OperationsTest, translate_pt)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto res = translate(box, Point(1, 0, 0), Point(2, 0, 0));
}

TEST(OperationsTest, scale)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto res = scale(box, 0.1);
    EXPECT_NEAR(res.volume(), 0.006, 1e-10);
}

TEST(OperationsTest, mirror)
{
    Axis1 ax(Point(5, 0, 0), Vector(1, 0, 0));
    auto box = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto res = mirror(box, ax);
    EXPECT_NEAR(std::abs(box.volume() - res.volume()), 0, 1e-10);
}

TEST(OperationsTest, fuse)
{
    auto box1 = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto box2 = Box::create(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = fuse(box1, box2);
    EXPECT_NEAR(res.volume(), 17.25, 1e-10);
}

TEST(OperationsTest, fuse_multiple)
{
    auto box1 = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto box2 = Box::create(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto box3 = Box::create(Point(1, 1, 0), Point(2, 2, -1));
    auto res = fuse({ box1, box2, box3 });
    EXPECT_NEAR(res.volume(), 18.25, 1e-10);
}

TEST(OperationsTest, cut)
{
    auto box1 = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto box2 = Box::create(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = cut(box1, box2);
    EXPECT_NEAR(res.volume(), 4.125, 1e-10);
}

TEST(OperationsTest, intersect)
{
    auto box1 = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    auto box2 = Box::create(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = intersect(box1, box2);
    EXPECT_NEAR(res.volume(), 1.875, 1e-10);
}

TEST(OperationsTest, combine_meshes)
{
    std::vector<Point> pts2d = { Point(0.0, 0.0), Point(0.5, 0.0), Point(1.0, 0.0),
                                 Point(0.0, 0.5), Point(0.5, 0.5), Point(1.0, 0.5),
                                 Point(0.0, 1.0), Point(0.5, 1.0), Point(1.0, 1.0) };
    std::vector<Element> elems2d = {
        Element::Quad4({ 0, 1, 4, 3 }),
        Element::Quad4({ 1, 2, 5, 4 }),
        Element::Quad4({ 3, 4, 7, 6 }),
        Element::Quad4({ 4, 5, 8, 7 }),
    };

    Mesh sq(pts2d, elems2d);
    sq.set_up();
    sq.set_cell_set(100, { 0, 2, 3 });
    sq.set_cell_set(101, { 1 });
    sq.set_cell_set_name(0, "blue");
    sq.set_cell_set_name(1, "grey");

    std::vector<Mesh> parts;
    parts.push_back(sq.translated(0, 0, 0));
    parts.push_back(sq.translated(1, 0, 0));
    parts.push_back(sq.translated(0, 1, 0));
    parts.push_back(sq.translated(1, 1, 0));

    auto m = combine(parts);

    EXPECT_EQ(m.num_elements(), 16);
    EXPECT_EQ(m.num_points(), 36);

    auto cs_ids = m.cell_set_ids();
    EXPECT_THAT(cs_ids, ElementsAre(100, 101));

    auto cs100 = m.cell_set(100);
    EXPECT_THAT(cs100, ElementsAre(0, 2, 3, 4, 6, 7, 8, 10, 11, 12, 14, 15));

    auto cs101 = m.cell_set(101);
    EXPECT_THAT(cs101, ElementsAre(1, 5, 9, 13));
}

TEST(OperationsTest, extrude)
{
    auto circ = Circle::create(Point(0, 0, 0), 2);
    auto res = extrude(circ, Vector(1, 1, 0));
}

TEST(OperationsTest, revolve)
{
    auto circ = Circle::create(Point(0, 0, 0), 2);
    Axis1 ax(Point(5, 0, 0), Vector(0, 1, 0));
    auto res = revolve(circ, ax);
}

TEST(OperationsTest, rotate_shape)
{
    auto a = Box::create(Point(2, 0, 0), Point(3, 2, 3));
    auto b = rotate(a, Axis1(Point(0, 0, 0), Vector(0, 0, 1)), radians(45));
}

TEST(OperationsTest, section)
{
    auto box1 = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    Plane pln(Point(0.5, 0, 0), Vector(1, 0, 0));
    auto res = section(box1, pln);
}

TEST(OperationsTest, draft)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 2, 3));
    std::vector<GeomSurface> faces;
    for (auto & f : box.surfaces()) {
        auto pln = Plane::create(f);
        auto n = pln.axis().direction();
        if (std::abs(n.z) < 1e-14)
            faces.emplace_back(f);
    }

    Plane neutral_plane(Point(0, 0, 0), Vector(0, 0, 1));
    auto shape = draft(box, neutral_plane, faces, radians(3));
}

TEST(OperationsTest, hole)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 1, 1));
    Axis1 ax1(Point(1.5, 0.5, 0.25), Vector(-1, 0, 0));
    auto shape = hole(box, ax1, 0.2);
}

TEST(OperationsTest, hole_blind)
{
    auto box = Box::create(Point(0, 0, 0), Point(1, 1, 1));
    Axis1 ax1(Point(0, 0.5, 0.25), Vector(1, 0, 0));
    auto shape = hole(box, ax1, 0.2, 0.5);
}

TEST(OperationsTest, sweep)
{
    auto l1 = Line::create(Point(0, 0, 0), Point(0, 1, 0));
    auto arc = ArcOfCircle::create(Point(0, 1, 0), Vector(0, 1, 0), Point(1, 2, 0));
    auto l2 = Line::create(Point(1, 2, 0), Point(2, 2, 0));
    auto path = Wire::create({ l1, arc, l2 });

    auto circ = Circle::create(Axis2(Point(0, 0, 0), Vector(0, 1, 0)), 0.1);
    auto profile = GeomSurface::create(Wire::create({ circ }));

    auto shape = sweep(profile, path);
}
