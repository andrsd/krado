#include "gmock/gmock.h"
#include "krado/element.h"
#include <array>

using namespace krado;

TEST(ElementTest, line2)
{
    auto elem = Element::Line2({ 1, 2 }, 123);
    EXPECT_EQ(elem.num_vertices(), 2);
    EXPECT_EQ(elem.type(), Element::LINE2);
    EXPECT_EQ(elem.marker(), 123);
    EXPECT_EQ(elem.vertex_id(0), 1);
    EXPECT_EQ(elem(1), 2);
}

TEST(MeshElementTest, tri3)
{
    auto elem = Element::Tri3({ 1, 2, 5 }, 123);
    EXPECT_EQ(elem.num_vertices(), 3);
    EXPECT_EQ(elem.type(), Element::TRI3);
    EXPECT_EQ(elem.marker(), 123);
    EXPECT_EQ(elem(0), 1);
    EXPECT_EQ(elem(1), 2);
    EXPECT_EQ(elem(2), 5);
}

TEST(MeshElementTest, tet4)
{
    auto elem = Element::Tetra4({ 2, 5, 6, 8 }, 123);
    EXPECT_EQ(elem.num_vertices(), 4);
    EXPECT_EQ(elem.type(), Element::TETRA4);
    EXPECT_EQ(elem.marker(), 123);
    EXPECT_EQ(elem(0), 2);
    EXPECT_EQ(elem(1), 5);
    EXPECT_EQ(elem(2), 6);
    EXPECT_EQ(elem(3), 8);
}

TEST(MeshElementTest, str_type)
{
    EXPECT_EQ(Element::type(Element::LINE2), "LINE2");
    EXPECT_EQ(Element::type(Element::TRI3), "TRI3");
    EXPECT_EQ(Element::type(Element::TETRA4), "TETRA4");
}
