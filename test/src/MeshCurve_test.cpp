#include "gmock/gmock.h"
#include "krado/geom_curve.h"
#include "krado/mesh.h"
#include "krado/mesh_curve.h"
#include "krado/exception.h"
#include "krado/scheme_equal.h"
#include "BRepLib_MakeEdge.hxx"

using namespace krado;

namespace {

TopoDS_Edge
build_line(Point pt1, Point pt2)
{
    gp_Pnt pnt1(pt1.x, pt1.y, pt1.z);
    gp_Pnt pnt2(pt2.x, pt2.y, pt2.z);
    BRepLib_MakeEdge make_edge(pnt1, pnt2);
    make_edge.Build();
    return make_edge.Edge();
}

} // namespace

TEST(MeshCurveTest, api) {
    auto edge = build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve gcurve(edge);

    MeshVertex v1(gcurve.first_vertex());
    MeshVertex v2(gcurve.last_vertex());
    MeshCurve mcurve(gcurve, &v1, &v2);

    EXPECT_EQ(&mcurve.geom_curve(), &gcurve);

    auto & mpars = mcurve.meshing_parameters();
    EXPECT_EQ(mpars.get<std::string>("scheme"), "auto");
    EXPECT_EQ(mpars.get<int>("marker"), 0);

    auto & vtx = mcurve.bounding_vertices();
    ASSERT_EQ(vtx.size(), 2);
    EXPECT_EQ(vtx[0], &v1);
    EXPECT_EQ(vtx[1], &v2);
}

TEST(MeshCurveTest, mesh)
{
    auto edge = build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomCurve gcurve(edge);
    auto gvtx1 = gcurve.first_vertex();
    auto gvtx2 = gcurve.last_vertex();

    MeshVertex v1(gvtx1);
    MeshVertex v2(gvtx2);
    MeshCurve mcurve(gcurve, &v1, &v2);

    Mesh mesh;
    Parameters pars;
    pars.set<int>("intervals") = 4;
    SchemeEqual equal(mesh, pars);
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
    EXPECT_EQ(&vtx2->geom_curve(), &gcurve);

    auto segs = mcurve.segments();
    EXPECT_EQ(segs.size(), 4);
    EXPECT_EQ(segs[0].type(), MeshElement::LINE2);
    EXPECT_EQ(segs[0].num_vertices(), 2);
    EXPECT_EQ(segs[0].vertex_id(0), 0);
    EXPECT_EQ(segs[0].vertex_id(1), 1);

    EXPECT_EQ(segs[3].type(), MeshElement::LINE2);
    EXPECT_EQ(segs[3].num_vertices(), 2);
    EXPECT_EQ(segs[3].vertex_id(0), 3);
    EXPECT_EQ(segs[3].vertex_id(1), 4);
}
