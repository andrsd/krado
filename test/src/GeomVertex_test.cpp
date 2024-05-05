#include "gmock/gmock.h"
#include "krado/geom_vertex.h"
#include "BRepLib_MakeVertex.hxx"

using namespace krado;

TEST(GeomVertexTest, ctor)
{
    gp_Pnt pt(1, 2, 3);
    BRepLib_MakeVertex make_vtx(pt);
    make_vtx.Build();
    auto vtx = make_vtx.Vertex();

    GeomVertex v(vtx);
    EXPECT_DOUBLE_EQ(v.x(), 1.);
    EXPECT_DOUBLE_EQ(v.y(), 2.);
    EXPECT_DOUBLE_EQ(v.z(), 3.);
}
