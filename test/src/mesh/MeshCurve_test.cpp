#include "gmock/gmock.h"
#include "krado/geo/curve.h"
#include "krado/mesh/curve.h"
#include "krado/mesh/scheme/equal.h"
#include "BRepLib_MakeEdge.hxx"
#include "eigen3/Eigen/Sparse"
#include "eigen3/Eigen/SparseLU"

using namespace krado;

TEST(Mesh_CurveTest, mesh)
{
    gp_Pnt pt1(0, 0, 0);
    gp_Pnt pt2(3, 4, 0);
    BRepLib_MakeEdge make_edge(pt1, pt2);
    make_edge.Build();
    geo::Curve gcurve(make_edge.Edge());

    mesh::Equal equal(4);
    auto mcurve = equal.mesh_curve(gcurve);
}
