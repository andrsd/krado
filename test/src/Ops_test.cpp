#include "gmock/gmock.h"
#include "krado/ops.h"
#include "krado/point.h"
#include "krado/box.h"
#include "krado/axis1.h"
#include "krado/mesh.h"
#include "krado/element.h"

using namespace krado;
using namespace testing;

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
