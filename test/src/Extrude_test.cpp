#include "gmock/gmock.h"
#include "krado/extrude.h"
#include "krado/mesh.h"

using namespace krado;
using namespace testing;

TEST(ExtrudeTest, DISABLED_line_1d)
{
    std::vector<Point> pts1d = { Point(0.0), Point(0.1), Point(0.2), Point(0.3) };
    std::vector<Element> elems1d = {
        Element::Line2({ 0, 1 }),
        Element::Line2({ 1, 2 }),
        Element::Line2({ 2, 3 }),
    };
    Mesh line(pts1d, elems1d);
    line.set_cell_set(0, { 0, 1 });
    line.set_cell_set(1, { 2 });
#if 0
    line.set_side_set(10, std::vector<side_set_entry_t> { { 0, 0 } });
    line.set_side_set(11, std::vector<side_set_entry_t> { { 2, 1 } });
#endif

    Mesh rectangle = extrude(line, Vector(0.0, 1.0), 2, 0.4);

    auto & pnts = rectangle.points();
    EXPECT_EQ(pnts.size(), 12);
    EXPECT_EQ(pnts[0], Point(0.0, 0.0));
    EXPECT_EQ(pnts[1], Point(0.1, 0.0));
    EXPECT_EQ(pnts[2], Point(0.2, 0.0));
    EXPECT_EQ(pnts[3], Point(0.3, 0.0));
    EXPECT_EQ(pnts[4], Point(0.0, 0.2));
    EXPECT_EQ(pnts[5], Point(0.1, 0.2));
    EXPECT_EQ(pnts[6], Point(0.2, 0.2));
    EXPECT_EQ(pnts[7], Point(0.3, 0.2));
    EXPECT_EQ(pnts[8], Point(0.0, 0.4));
    EXPECT_EQ(pnts[9], Point(0.1, 0.4));
    EXPECT_EQ(pnts[10], Point(0.2, 0.4));
    EXPECT_EQ(pnts[11], Point(0.3, 0.4));

    auto & elems = rectangle.elements();
    EXPECT_EQ(elems.size(), 6);
    EXPECT_EQ(elems[0], Element::Quad4({ 0, 1, 5, 4 }));
    EXPECT_EQ(elems[1], Element::Quad4({ 1, 2, 6, 5 }));
    EXPECT_EQ(elems[2], Element::Quad4({ 2, 3, 7, 6 }));
    EXPECT_EQ(elems[3], Element::Quad4({ 4, 5, 9, 8 }));
    EXPECT_EQ(elems[4], Element::Quad4({ 5, 6, 10, 9 }));
    EXPECT_EQ(elems[5], Element::Quad4({ 6, 7, 11, 10 }));

    auto cs_ids = rectangle.cell_set_ids();
    EXPECT_THAT(cs_ids, UnorderedElementsAre(0, 1));
    EXPECT_THAT(rectangle.cell_set(0), UnorderedElementsAre(0, 1, 3, 4));
    EXPECT_THAT(rectangle.cell_set(1), UnorderedElementsAre(2, 5));

#if 0
    auto ss_ids = rectangle.side_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = rectangle.side_set(10);
    EXPECT_EQ(ss0[0], side_set_entry_t(0, 3));
    EXPECT_EQ(ss0[1], side_set_entry_t(3, 3));

    auto & ss1 = rectangle.side_set(11);
    EXPECT_EQ(ss1[0], side_set_entry_t(2, 1));
    EXPECT_EQ(ss1[1], side_set_entry_t(5, 1));
#endif
}

TEST(ExtrudeTest, tri_2d)
{
    std::vector<Point> pts2d = { Point(0.0, 0.0),
                                 Point(1.0, 0.0),
                                 Point(1.0, 1.0),
                                 Point(0.0, 1.0),
                                 Point(0.5, 0.5) };
    std::vector<Element> elems2d = {
        Element::Tri3({ 0, 1, 4 }),
        Element::Tri3({ 1, 2, 4 }),
        Element::Tri3({ 2, 3, 4 }),
        Element::Tri3({ 3, 0, 4 }),
    };
    Mesh square(pts2d, elems2d);
    square.set_up();
    square.set_cell_set(0, { 0, 1, 3 });
    square.set_cell_set(1, { 2 });
    square.set_edge_set(10, std::vector<gidx_t> { 9 });
    square.set_edge_set(11, std::vector<gidx_t> { 14 });

    Mesh box = extrude(square, Vector(0.0, 0.0, 1.0), 2, 0.4);

    auto & pnts = box.points();
    EXPECT_EQ(pnts.size(), 15);
    EXPECT_EQ(pnts[0], Point(0.0, 0.0, 0.0));
    EXPECT_EQ(pnts[1], Point(1.0, 0.0, 0.0));
    EXPECT_EQ(pnts[2], Point(1.0, 1.0, 0.0));
    EXPECT_EQ(pnts[3], Point(0.0, 1.0, 0.0));
    EXPECT_EQ(pnts[4], Point(0.5, 0.5, 0.0));
    EXPECT_EQ(pnts[5], Point(0.0, 0.0, 0.2));
    EXPECT_EQ(pnts[6], Point(1.0, 0.0, 0.2));
    EXPECT_EQ(pnts[7], Point(1.0, 1.0, 0.2));
    EXPECT_EQ(pnts[8], Point(0.0, 1.0, 0.2));
    EXPECT_EQ(pnts[9], Point(0.5, 0.5, 0.2));
    EXPECT_EQ(pnts[10], Point(0.0, 0.0, 0.4));
    EXPECT_EQ(pnts[11], Point(1.0, 0.0, 0.4));
    EXPECT_EQ(pnts[12], Point(1.0, 1.0, 0.4));
    EXPECT_EQ(pnts[13], Point(0.0, 1.0, 0.4));
    EXPECT_EQ(pnts[14], Point(0.5, 0.5, 0.4));

    auto & elems = box.elements();
    EXPECT_EQ(elems.size(), 8);
    EXPECT_EQ(elems[0], Element::Prism6({ 0, 1, 4, 5, 6, 9 }));
    EXPECT_EQ(elems[1], Element::Prism6({ 1, 2, 4, 6, 7, 9 }));
    EXPECT_EQ(elems[2], Element::Prism6({ 2, 3, 4, 7, 8, 9 }));
    EXPECT_EQ(elems[3], Element::Prism6({ 3, 0, 4, 8, 5, 9 }));
    EXPECT_EQ(elems[4], Element::Prism6({ 5, 6, 9, 10, 11, 14 }));
    EXPECT_EQ(elems[5], Element::Prism6({ 6, 7, 9, 11, 12, 14 }));
    EXPECT_EQ(elems[6], Element::Prism6({ 7, 8, 9, 12, 13, 14 }));
    EXPECT_EQ(elems[7], Element::Prism6({ 8, 5, 9, 13, 10, 14 }));

    auto cs_ids = box.cell_set_ids();
    EXPECT_THAT(cs_ids, UnorderedElementsAre(0, 1));
    EXPECT_THAT(box.cell_set(0), UnorderedElementsAre(0, 1, 3, 4, 5, 7));
    EXPECT_THAT(box.cell_set(1), UnorderedElementsAre(2, 6));

    auto ss_ids = box.face_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11));

    auto & ss0 = box.face_set(10);
    EXPECT_THAT(ss0, testing::ElementsAre(24, 39));

    auto & ss1 = box.face_set(11);
    EXPECT_THAT(ss1, testing::ElementsAre(33, 46));
}

TEST(ExtrudeTest, quad_2d)
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

    Mesh square(pts2d, elems2d);
    square.set_up();
    square.set_cell_set(0, { 0, 2, 3 });
    square.set_cell_set(1, { 1 });
    square.set_edge_set(10, std::vector<gidx_t> { 13, 17 });
    square.set_edge_set(11, std::vector<gidx_t> { 21, 24 });
    square.set_edge_set(12, std::vector<gidx_t> { 18, 23 });
    square.set_edge_set(13, std::vector<gidx_t> { 16, 22 });

    auto box = extrude(square, Vector(0.0, 0.0, 1.0), 2, 0.4);

    auto & pnts = box.points();
    EXPECT_EQ(pnts.size(), 27);
    EXPECT_EQ(pnts[0], Point(0.0, 0.0, 0.0));
    EXPECT_EQ(pnts[1], Point(0.5, 0.0, 0.0));
    EXPECT_EQ(pnts[2], Point(1.0, 0.0, 0.0));
    EXPECT_EQ(pnts[3], Point(0.0, 0.5, 0.0));
    EXPECT_EQ(pnts[4], Point(0.5, 0.5, 0.0));
    EXPECT_EQ(pnts[5], Point(1.0, 0.5, 0.0));
    EXPECT_EQ(pnts[6], Point(0.0, 1.0, 0.0));
    EXPECT_EQ(pnts[7], Point(0.5, 1.0, 0.0));
    EXPECT_EQ(pnts[8], Point(1.0, 1.0, 0.0));
    EXPECT_EQ(pnts[9], Point(0.0, 0.0, 0.2));
    EXPECT_EQ(pnts[10], Point(0.5, 0.0, 0.2));
    EXPECT_EQ(pnts[11], Point(1.0, 0.0, 0.2));
    EXPECT_EQ(pnts[12], Point(0.0, 0.5, 0.2));
    EXPECT_EQ(pnts[13], Point(0.5, 0.5, 0.2));
    EXPECT_EQ(pnts[14], Point(1.0, 0.5, 0.2));
    EXPECT_EQ(pnts[15], Point(0.0, 1.0, 0.2));
    EXPECT_EQ(pnts[16], Point(0.5, 1.0, 0.2));
    EXPECT_EQ(pnts[17], Point(1.0, 1.0, 0.2));
    EXPECT_EQ(pnts[18], Point(0.0, 0.0, 0.4));
    EXPECT_EQ(pnts[19], Point(0.5, 0.0, 0.4));
    EXPECT_EQ(pnts[20], Point(1.0, 0.0, 0.4));
    EXPECT_EQ(pnts[21], Point(0.0, 0.5, 0.4));
    EXPECT_EQ(pnts[22], Point(0.5, 0.5, 0.4));
    EXPECT_EQ(pnts[23], Point(1.0, 0.5, 0.4));
    EXPECT_EQ(pnts[24], Point(0.0, 1.0, 0.4));
    EXPECT_EQ(pnts[25], Point(0.5, 1.0, 0.4));
    EXPECT_EQ(pnts[26], Point(1.0, 1.0, 0.4));

    auto & elems = box.elements();
    EXPECT_EQ(elems.size(), 8);
    EXPECT_EQ(elems[0], Element::Hex8({ 0, 1, 4, 3, 9, 10, 13, 12 }));
    EXPECT_EQ(elems[1], Element::Hex8({ 1, 2, 5, 4, 10, 11, 14, 13 }));
    EXPECT_EQ(elems[2], Element::Hex8({ 3, 4, 7, 6, 12, 13, 16, 15 }));
    EXPECT_EQ(elems[3], Element::Hex8({ 4, 5, 8, 7, 13, 14, 17, 16 }));
    EXPECT_EQ(elems[4], Element::Hex8({ 9, 10, 13, 12, 18, 19, 22, 21 }));
    EXPECT_EQ(elems[5], Element::Hex8({ 10, 11, 14, 13, 19, 20, 23, 22 }));
    EXPECT_EQ(elems[6], Element::Hex8({ 12, 13, 16, 15, 21, 22, 25, 24 }));
    EXPECT_EQ(elems[7], Element::Hex8({ 13, 14, 17, 16, 22, 23, 26, 25 }));

    auto cs_ids = box.cell_set_ids();
    EXPECT_THAT(cs_ids, UnorderedElementsAre(0, 1));
    EXPECT_THAT(box.cell_set(0), UnorderedElementsAre(0, 2, 3, 4, 6, 7));
    EXPECT_THAT(box.cell_set(1), UnorderedElementsAre(1, 5));

    auto ss_ids = box.face_set_ids();
    EXPECT_THAT(ss_ids, ElementsAre(10, 11, 12, 13));

    auto & ss0 = box.face_set(10);
    EXPECT_THAT(ss0, testing::ElementsAre(35, 41, 55, 60));

    auto & ss1 = box.face_set(11);
    EXPECT_THAT(ss1, testing::ElementsAre(46, 51, 64, 68));

    auto & ss2 = box.face_set(12);
    EXPECT_THAT(ss2, testing::ElementsAre(43, 52, 62, 69));

    auto & ss3 = box.face_set(13);
    EXPECT_THAT(ss3, testing::ElementsAre(37, 47, 57, 65));
}
