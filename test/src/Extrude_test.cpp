#include "gmock/gmock.h"
#include "krado/extrude.h"

using namespace krado;

TEST(ExtrudeTest, line_1d)
{
    std::vector<Point> pts1d = { Point(0.0), Point(0.1), Point(0.2), Point(0.3) };
    std::vector<Element> elems1d = {
        Element::Line2({ 0, 1 }, 0),
        Element::Line2({ 1, 2 }, 0),
        Element::Line2({ 2, 3 }, 1),
    };
    Mesh line(pts1d, elems1d);

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
    EXPECT_EQ(elems[0], Element::Quad4({ 0, 1, 5, 4 }, 0));
    EXPECT_EQ(elems[1], Element::Quad4({ 1, 2, 6, 5 }, 0));
    EXPECT_EQ(elems[2], Element::Quad4({ 2, 3, 7, 6 }, 1));
    EXPECT_EQ(elems[3], Element::Quad4({ 4, 5, 9, 8 }, 0));
    EXPECT_EQ(elems[4], Element::Quad4({ 5, 6, 10, 9 }, 0));
    EXPECT_EQ(elems[5], Element::Quad4({ 6, 7, 11, 10 }, 1));
}

TEST(ExtrudeTest, tri_2d)
{
    std::vector<Point> pts2d = { Point(0.0, 0.0),
                                 Point(1.0, 0.0),
                                 Point(1.0, 1.0),
                                 Point(0.0, 1.0),
                                 Point(0.5, 0.5) };
    std::vector<Element> elems2d = {
        Element::Tri3({ 0, 1, 4 }, 0),
        Element::Tri3({ 1, 2, 4 }, 0),
        Element::Tri3({ 2, 3, 4 }, 1),
        Element::Tri3({ 3, 0, 4 }, 0),
    };
    Mesh square(pts2d, elems2d);

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
    EXPECT_EQ(elems[0], Element::Prism6({ 0, 1, 4, 5, 6, 9 }, 0));
    EXPECT_EQ(elems[1], Element::Prism6({ 1, 2, 4, 6, 7, 9 }, 0));
    EXPECT_EQ(elems[2], Element::Prism6({ 2, 3, 4, 7, 8, 9 }, 1));
    EXPECT_EQ(elems[3], Element::Prism6({ 3, 0, 4, 8, 5, 9 }, 0));
    EXPECT_EQ(elems[4], Element::Prism6({ 5, 6, 9, 10, 11, 14 }, 0));
    EXPECT_EQ(elems[5], Element::Prism6({ 6, 7, 9, 11, 12, 14 }, 0));
    EXPECT_EQ(elems[6], Element::Prism6({ 7, 8, 9, 12, 13, 14 }, 1));
    EXPECT_EQ(elems[7], Element::Prism6({ 8, 5, 9, 13, 10, 14 }, 0));
}

TEST(ExtrudeTest, quad_2d)
{
    std::vector<Point> pts2d = { Point(0.0, 0.0), Point(0.5, 0.0), Point(1.0, 0.0),
                                 Point(0.0, 0.5), Point(0.5, 0.5), Point(1.0, 0.5),
                                 Point(0.0, 1.0), Point(0.5, 1.0), Point(1.0, 1.0) };
    std::vector<Element> elems2d = {
        Element::Quad4({ 0, 1, 4, 3 }, 0),
        Element::Quad4({ 1, 2, 5, 4 }, 1),
        Element::Quad4({ 3, 4, 7, 6 }, 0),
        Element::Quad4({ 4, 5, 8, 7 }, 0),
    };

    Mesh square(pts2d, elems2d);

    Mesh box = extrude(square, Vector(0.0, 0.0, 1.0), 2, 0.4);

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
    EXPECT_EQ(elems[0], Element::Hex8({ 0, 1, 4, 3, 9, 10, 13, 12 }, 0));
    EXPECT_EQ(elems[1], Element::Hex8({ 1, 2, 5, 4, 10, 11, 14, 13 }, 1));
    EXPECT_EQ(elems[2], Element::Hex8({ 3, 4, 7, 6, 12, 13, 16, 15 }, 0));
    EXPECT_EQ(elems[3], Element::Hex8({ 4, 5, 8, 7, 13, 14, 17, 16 }, 0));
    EXPECT_EQ(elems[4], Element::Hex8({ 9, 10, 13, 12, 18, 19, 22, 21 }, 0));
    EXPECT_EQ(elems[5], Element::Hex8({ 10, 11, 14, 13, 19, 20, 23, 22 }, 1));
    EXPECT_EQ(elems[6], Element::Hex8({ 12, 13, 16, 15, 21, 22, 25, 24 }, 0));
    EXPECT_EQ(elems[7], Element::Hex8({ 13, 14, 17, 16, 22, 23, 26, 25 }, 0));
}
