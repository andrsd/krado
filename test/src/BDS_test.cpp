#include "gmock/gmock.h"
#include "krado/bds.h"

using namespace krado;

TEST(BDSMeshTest, empty_mesh)
{
    BDS_Mesh m;
    EXPECT_EQ(m.points().size(), 0);
    EXPECT_EQ(m.edges().size(), 0);
    EXPECT_EQ(m.triangles().size(), 0);
}

TEST(BDSMeshTest, points)
{
    BDS_Mesh m;
    m.add_point(0, Point(0, 0));
    m.add_point(1, Point(2, 0));
    auto p3 = m.add_point(2, Point(2, 1));
    m.add_point(3, Point(0, 1));

    EXPECT_EQ(m.points().size(), 4);

    m.del_point(p3);

    EXPECT_EQ(m.points().size(), 3);
}

TEST(BDSMeshTest, edges)
{
    BDS_Mesh m;
    auto pt1 = m.add_point(1, Point(0, 0));
    auto pt2 = m.add_point(2, Point(2, 0));
    auto pt3 = m.add_point(3, Point(2, 1));
    auto pt4 = m.add_point(4, Point(0, 1));

    m.add_edge(1, 2);
    m.add_edge(2, 3);
    m.add_edge(3, 1);
    auto e4_v = m.add_edge(3, 4);
    m.add_edge(4, 1);

    EXPECT_EQ(m.points().size(), 4);
    EXPECT_EQ(m.edges().size(), 5);

    ASSERT_TRUE(e4_v.has_value());
    auto e4 = e4_v.value();
    m.del_edge(e4);
    EXPECT_TRUE(e4->deleted());
}
