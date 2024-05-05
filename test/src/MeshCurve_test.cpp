#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/equal.h"
#include "BRepLib_MakeEdge.hxx"

using namespace krado;

TEST(MeshCurveTest, mesh)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    GeomCurve gcurve(make_edge.Edge());
    auto gvtx1 = gcurve.first_vertex();
    auto gvtx2 = gcurve.last_vertex();

    MeshVertex v1(gvtx1);
    MeshVertex v2(gvtx2);
    MeshCurve mcurve(gcurve, &v1, &v2);

    Equal equal(4);
    equal.mesh_curve(mcurve);

    auto vtx_first = mcurve.vertex(0);
    EXPECT_EQ(&vtx_first.geom_vertex(), &gvtx1);

    auto vtx_last = mcurve.vertex(1);
    EXPECT_EQ(&vtx_last.geom_vertex(), &gvtx2);

    auto vtx = mcurve.curve_vertices();
    EXPECT_EQ(vtx.size(), 3);
    auto vtx2 = vtx[1];
    EXPECT_DOUBLE_EQ(vtx2.parameter(), 2.5);
    EXPECT_DOUBLE_EQ(vtx2.point().x, 1.5);
    EXPECT_DOUBLE_EQ(vtx2.point().y, 2.0);
    EXPECT_EQ(&vtx2.geom_curve(), &gcurve);

    auto segs = mcurve.curve_segments();
    EXPECT_EQ(segs.size(), 4);
    EXPECT_EQ(segs[0].type(), MeshElement::LINE2);
    EXPECT_EQ(segs[0].num_vertices(), 2);
    EXPECT_EQ(segs[0].vertex_id(0), MeshCurve::FIRST_VERTEX);
    EXPECT_EQ(segs[0].vertex_id(1), 0);

    EXPECT_EQ(segs[3].type(), MeshElement::LINE2);
    EXPECT_EQ(segs[3].num_vertices(), 2);
    EXPECT_EQ(segs[3].vertex_id(0), 2);
    EXPECT_EQ(segs[3].vertex_id(1), MeshCurve::LAST_VERTEX);
}
