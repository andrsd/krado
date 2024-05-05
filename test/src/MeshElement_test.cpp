#include "gmock/gmock.h"
#include "krado/mesh_element.h"

using namespace krado;

TEST(MeshElementTest, line2)
{
    auto elem = MeshElement::Line2(1, 2);
    EXPECT_EQ(elem.num_vertices(), 2);
    EXPECT_EQ(elem.type(), MeshElement::LINE2);
}

TEST(MeshElementTest, tri3)
{
    auto elem = MeshElement::Tri3(1, 2, 5);
    EXPECT_EQ(elem.num_vertices(), 3);
    EXPECT_EQ(elem.type(), MeshElement::TRI3);
}

TEST(MeshElementTest, tet4)
{
    auto elem = MeshElement::Tetra4(2, 5, 6, 8);
    EXPECT_EQ(elem.num_vertices(), 4);
    EXPECT_EQ(elem.type(), MeshElement::TETRA4);
}
