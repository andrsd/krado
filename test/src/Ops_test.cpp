#include "gmock/gmock.h"
#include "krado/ops.h"
#include "krado/point.h"
#include "krado/box.h"
#include "krado/mesh.h"
#include "krado/element.h"

using namespace krado;
using namespace testing;

TEST(OperationsTest, fuse)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = fuse(box1, box2);
}

TEST(OperationsTest, fuse_multiple)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    Box box3(Point(1, 1, 0), Point(2, 2, -1));
    auto res = fuse({ box1, box2, box3 });
}

TEST(OperationsTest, cut)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = cut(box1, box2);
}

TEST(OperationsTest, intersect)
{
    Box box1(Point(0, 0, 0), Point(1, 2, 3));
    Box box2(Point(0.5, 0.5, 0.5), Point(2, 3, 4));
    auto res = intersect(box1, box2);
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
