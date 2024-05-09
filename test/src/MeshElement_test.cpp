#include "gmock/gmock.h"
#include "krado/mesh_element.h"

using namespace krado;

TEST(MeshElementTest, line2)
{
    auto elem = MeshElement::Line2(1, 2);
    EXPECT_EQ(elem.num_vertices(), 2);
    EXPECT_EQ(elem.type(), MeshElement::LINE2);
    EXPECT_EQ(elem.vertex_id(0), 1);
    EXPECT_EQ(elem(1), 2);
}

TEST(MeshElementTest, tri3)
{
    auto elem = MeshElement::Tri3(1, 2, 5);
    EXPECT_EQ(elem.num_vertices(), 3);
    EXPECT_EQ(elem.type(), MeshElement::TRI3);
    EXPECT_EQ(elem(0), 1);
    EXPECT_EQ(elem(1), 2);
    EXPECT_EQ(elem(2), 5);
}

TEST(MeshElementTest, tet4)
{
    auto elem = MeshElement::Tetra4(2, 5, 6, 8);
    EXPECT_EQ(elem.num_vertices(), 4);
    EXPECT_EQ(elem.type(), MeshElement::TETRA4);
    EXPECT_EQ(elem(0), 2);
    EXPECT_EQ(elem(1), 5);
    EXPECT_EQ(elem(2), 6);
    EXPECT_EQ(elem(3), 8);
}
