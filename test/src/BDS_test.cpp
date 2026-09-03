#include "gmock/gmock.h"
#include "krado/bds.h"

using namespace krado;

TEST(BDSMeshTest, empty_mesh)
{
    BDS_Mesh m;
    EXPECT_EQ(m.points().size(), 0);
    EXPECT_EQ(m.edges().size(), 0);
    EXPECT_EQ(m.faces().size(), 0);
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
}

TEST(BDSMeshTest, edges)
{
    BDS_Mesh m;
    auto pt1 = m.add_point(1, Point(0, 0));
    auto pt2 = m.add_point(2, Point(2, 0));
    auto pt3 = m.add_point(3, Point(2, 1));
    auto pt4 = m.add_point(4, Point(0, 1));

    m.add_edge(pt1, pt2);
    m.add_edge(pt2, pt3);
    m.add_edge(pt3, pt1);
    auto e4_v = m.add_edge(pt3, pt4);
    m.add_edge(pt4, pt1);

    EXPECT_EQ(m.points().size(), 4);
    EXPECT_EQ(m.edges().size(), 5);

    ASSERT_TRUE(e4_v.has_value());
    auto e4 = e4_v.value();
    m.del_edge(e4);
    auto & edge4 = m.get_edge(e4);
    EXPECT_FALSE(edge4.active);
}

TEST(BDSMeshTest, add_tris)
{
    BDS_Mesh m;
    auto pt1 = m.add_point(1, Point(0, 0));
    auto pt2 = m.add_point(2, Point(2, 0));
    auto pt3 = m.add_point(3, Point(2, 1));
    auto pt4 = m.add_point(4, Point(0, 1));

    m.add_face(pt1, pt2, pt3);
    m.add_face(pt3, pt4, pt1);

    {
        auto pt = m.get_point(pt1);
        ASSERT_EQ(pt.edges.size(), 3);
        EXPECT_THAT(pt.edges,
                    testing::UnorderedElementsAreArray(
                        { BDSEdgeHandle { 0 }, BDSEdgeHandle { 2 }, BDSEdgeHandle { 4 } }));
    }
    {
        auto pt = m.get_point(pt2);
        ASSERT_EQ(pt.edges.size(), 2);
        EXPECT_THAT(
            pt.edges,
            testing::UnorderedElementsAreArray({ BDSEdgeHandle { 0 }, BDSEdgeHandle { 1 } }));
    }
    {
        auto pt = m.get_point(pt3);
        ASSERT_EQ(pt.edges.size(), 3);
        EXPECT_THAT(pt.edges,
                    testing::UnorderedElementsAreArray(
                        { BDSEdgeHandle { 1 }, BDSEdgeHandle { 2 }, BDSEdgeHandle { 3 } }));
    }
    {
        auto pt = m.get_point(pt4);
        ASSERT_EQ(pt.edges.size(), 2);
        EXPECT_THAT(
            pt.edges,
            testing::UnorderedElementsAreArray({ BDSEdgeHandle { 3 }, BDSEdgeHandle { 4 } }));
    }

    {
        auto edge = m.get_edge(BDSEdgeHandle { 0 });
        EXPECT_EQ(edge.p1, BDSPointHandle { 0 });
        EXPECT_EQ(edge.p2, BDSPointHandle { 1 });
        EXPECT_THAT(edge.faces, testing::UnorderedElementsAreArray({ BDSFaceHandle { 0 } }));
    }
    {
        auto edge = m.get_edge(BDSEdgeHandle { 1 });
        EXPECT_EQ(edge.p1, BDSPointHandle { 1 });
        EXPECT_EQ(edge.p2, BDSPointHandle { 2 });
        EXPECT_THAT(edge.faces, testing::UnorderedElementsAreArray({ BDSFaceHandle { 0 } }));
    }
    {
        auto edge = m.get_edge(BDSEdgeHandle { 2 });
        EXPECT_EQ(edge.p1, BDSPointHandle { 0 });
        EXPECT_EQ(edge.p2, BDSPointHandle { 2 });
        EXPECT_THAT(
            edge.faces,
            testing::UnorderedElementsAreArray({ BDSFaceHandle { 0 }, BDSFaceHandle { 1 } }));
    }
    {
        auto edge = m.get_edge(BDSEdgeHandle { 3 });
        EXPECT_EQ(edge.p1, BDSPointHandle { 2 });
        EXPECT_EQ(edge.p2, BDSPointHandle { 3 });
        EXPECT_THAT(edge.faces, testing::UnorderedElementsAreArray({ BDSFaceHandle { 1 } }));
    }
    {
        auto edge = m.get_edge(BDSEdgeHandle { 4 });
        EXPECT_EQ(edge.p1, BDSPointHandle { 0 });
        EXPECT_EQ(edge.p2, BDSPointHandle { 3 });
        EXPECT_THAT(edge.faces, testing::UnorderedElementsAreArray({ BDSFaceHandle { 1 } }));
    }
}

TEST(BDSMeshTest, tris)
{
    BDS_Mesh m;
    auto pt1 = m.add_point(1, Point(0, 0));
    auto pt2 = m.add_point(2, Point(2, 0));
    auto pt3 = m.add_point(3, Point(2, 1));
    auto pt4 = m.add_point(4, Point(0, 1));

    auto e1 = m.add_edge(pt1, pt2);
    auto e2 = m.add_edge(pt2, pt3);
    auto e3 = m.add_edge(pt3, pt1);
    auto e4 = m.add_edge(pt3, pt4);
    auto e5 = m.add_edge(pt4, pt1);

    auto t1 = m.add_face(e1.value(), e2.value(), e3.value());
    auto t2 = m.add_face(e3.value(), e4.value(), e5.value());
    EXPECT_EQ(t1.value().id, 0);
    EXPECT_EQ(t2.value().id, 1);
}
