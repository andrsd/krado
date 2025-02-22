#include "gmock/gmock.h"
#include "krado/element.h"
#include <array>

using namespace krado;

TEST(ElementTest, line2)
{
    auto elem = Element::Line2({ 1, 2 });
    EXPECT_EQ(elem.num_vertices(), 2);
    EXPECT_EQ(elem.type(), ElementType::LINE2);
    EXPECT_EQ(elem.vertex_id(0), 1);
    EXPECT_EQ(elem(1), 2);
}

TEST(MeshElementTest, tri3)
{
    auto elem = Element::Tri3({ 1, 2, 5 });
    EXPECT_EQ(elem.num_vertices(), 3);
    EXPECT_EQ(elem.type(), ElementType::TRI3);
    EXPECT_EQ(elem(0), 1);
    EXPECT_EQ(elem(1), 2);
    EXPECT_EQ(elem(2), 5);
}

TEST(MeshElementTest, tet4)
{
    auto elem = Element::Tetra4({ 2, 5, 6, 8 });
    EXPECT_EQ(elem.num_vertices(), 4);
    EXPECT_EQ(elem.type(), ElementType::TETRA4);
    EXPECT_EQ(elem(0), 2);
    EXPECT_EQ(elem(1), 5);
    EXPECT_EQ(elem(2), 6);
    EXPECT_EQ(elem(3), 8);
}

TEST(MeshElementTest, str_type)
{
    EXPECT_EQ(Element::type(ElementType::LINE2), "LINE2");
    EXPECT_EQ(Element::type(ElementType::TRI3), "TRI3");
    EXPECT_EQ(Element::type(ElementType::TETRA4), "TETRA4");
}
