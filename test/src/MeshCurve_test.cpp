#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/scheme/equal.h"
#include "builder.h"

using namespace krado;

TEST(MeshCurveTest, api)
{
    auto edge = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));

    MeshVertex v1(edge.first_vertex());
    MeshVertex v2(edge.last_vertex());
    MeshCurve mcurve(edge, &v1, &v2);

    EXPECT_EQ(&mcurve.geom_curve(), &edge);

    EXPECT_EQ(mcurve.scheme().name(), "auto");

    auto & vtx = mcurve.bounding_vertices();
    ASSERT_EQ(vtx.size(), 2);
    EXPECT_EQ(vtx[0], &v1);
    EXPECT_EQ(vtx[1], &v2);
}

TEST(MeshCurveTest, mesh)
{
    auto edge = testing::build_line(Point(0, 0, 0), Point(3, 4, 0));
    auto gvtx1 = edge.first_vertex();
    auto gvtx2 = edge.last_vertex();

    MeshVertex v1(gvtx1);
    MeshVertex v2(gvtx2);
    MeshCurve mcurve(edge, &v1, &v2);

    SchemeEqual equal;
    equal.set("intervals", 4);
    equal.mesh_curve(mcurve);

    auto vtx = mcurve.bounding_vertices();
    ASSERT_EQ(vtx.size(), 2);
    EXPECT_EQ(&vtx[0]->geom_vertex(), &gvtx1);
    EXPECT_EQ(&vtx[1]->geom_vertex(), &gvtx2);

    auto curve_vtx = mcurve.curve_vertices();
    EXPECT_EQ(curve_vtx.size(), 3);
    auto vtx2 = curve_vtx[1];
    EXPECT_DOUBLE_EQ(vtx2->parameter(), 2.5);
    EXPECT_DOUBLE_EQ(vtx2->point().x, 1.5);
    EXPECT_DOUBLE_EQ(vtx2->point().y, 2.0);
    EXPECT_EQ(&vtx2->geom_curve(), &edge);

    auto segs = mcurve.segments();
    EXPECT_EQ(segs.size(), 4);
    EXPECT_EQ(segs[0].type(), ElementType::LINE2);
    EXPECT_EQ(segs[0].num_vertices(), 2);

    EXPECT_EQ(segs[3].type(), ElementType::LINE2);
    EXPECT_EQ(segs[3].num_vertices(), 2);
}
