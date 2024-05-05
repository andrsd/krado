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
}
