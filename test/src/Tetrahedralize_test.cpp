#include <gmock/gmock.h>
#include "krado/point.h"
#include "krado/tetrahedralize.h"

using namespace krado;
using namespace testing;

TEST(TetrahedralizeTest, pyramid5)
{
    std::vector<Point> pnts = { Point(0, 0, 0),
                                Point(1, 0, 0),
                                Point(1, 1, 0),
                                Point(0, 1, 0),
                                Point(0.5, 0.5, 1) };
    std::vector<Element> elems = { Element::Pyramid5({ 0, 1, 2, 3, 4 }, 12) };
    Mesh mesh(pnts, elems);

    Mesh tet_mesh = tetrahedralize(mesh);
    ASSERT_EQ(tet_mesh.elements().size(), 2);

    const auto & tet0 = tet_mesh.element(0);
    EXPECT_EQ(tet0.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet0.ids(), ElementsAre(0, 1, 2, 4));
    EXPECT_EQ(tet0.marker(), 12);

    const auto & tet1 = tet_mesh.element(1);
    EXPECT_EQ(tet1.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet1.ids(), ElementsAre(0, 2, 3, 4));
    EXPECT_EQ(tet1.marker(), 12);
}

TEST(TetrahedralizeTest, prism6)
{
    std::vector<Point> pnts = { Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0),
                                Point(0, 0, 1), Point(1, 0, 1), Point(0, 1, 1) };
    std::vector<Element> elems = { Element::Prism6({ 0, 1, 2, 3, 4, 5 }, 12) };
    Mesh mesh(pnts, elems);

    Mesh tet_mesh = tetrahedralize(mesh);
    ASSERT_EQ(tet_mesh.elements().size(), 3);

    const auto & tet0 = tet_mesh.element(0);
    EXPECT_EQ(tet0.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet0.ids(), ElementsAre(3, 5, 4, 0));
    EXPECT_EQ(tet0.marker(), 12);

    const auto & tet1 = tet_mesh.element(1);
    EXPECT_EQ(tet1.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet1.ids(), ElementsAre(1, 4, 5, 0));
    EXPECT_EQ(tet1.marker(), 12);

    const auto & tet2 = tet_mesh.element(2);
    EXPECT_EQ(tet2.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet2.ids(), ElementsAre(1, 5, 2, 0));
    EXPECT_EQ(tet2.marker(), 12);
}

TEST(TetrahedralizeTest, hex8)
{
    std::vector<Point> pnts = {
        Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0), Point(0, 1, 0),
        Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1),
    };
    std::vector<Element> elems = { Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 }, 12) };
    Mesh mesh(pnts, elems);

    Mesh tet_mesh = tetrahedralize(mesh);
    ASSERT_EQ(tet_mesh.elements().size(), 6);

    const auto & tet0 = tet_mesh.element(0);
    EXPECT_EQ(tet0.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet0.ids(), ElementsAre(0, 1, 2, 6));
    EXPECT_EQ(tet0.marker(), 12);

    const auto & tet1 = tet_mesh.element(1);
    EXPECT_EQ(tet1.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet1.ids(), ElementsAre(0, 5, 1, 6));
    EXPECT_EQ(tet1.marker(), 12);

    const auto & tet2 = tet_mesh.element(2);
    EXPECT_EQ(tet2.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet2.ids(), ElementsAre(0, 4, 5, 6));
    EXPECT_EQ(tet2.marker(), 12);

    const auto & tet3 = tet_mesh.element(3);
    EXPECT_EQ(tet3.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet3.ids(), ElementsAre(0, 2, 3, 7));
    EXPECT_EQ(tet3.marker(), 12);

    const auto & tet4 = tet_mesh.element(4);
    EXPECT_EQ(tet4.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet4.ids(), ElementsAre(0, 6, 2, 7));
    EXPECT_EQ(tet4.marker(), 12);

    const auto & tet5 = tet_mesh.element(5);
    EXPECT_EQ(tet5.type(), Element::Type::TETRA4);
    EXPECT_THAT(tet5.ids(), ElementsAre(0, 4, 6, 7));
    EXPECT_EQ(tet5.marker(), 12);
}
