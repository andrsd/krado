#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/geom_vertex.h"
#include "krado/mesh_vertex.h"
#include "BRepLib_MakeVertex.hxx"

using namespace krado;

namespace {

TopoDS_Vertex
build_vertex(Point pt1)
{
    gp_Pnt pnt1(pt1.x, pt1.y, pt1.z);
    BRepLib_MakeVertex make_vertex(pnt1);
    make_vertex.Build();
    return make_vertex.Vertex();
}

} // namespace

TEST(MeshVertexTest, api) {
    auto vtx = build_vertex(Point(3, 4, 0));
    GeomVertex gvertex(vtx);

    MeshVertex mvertex(gvertex);

    EXPECT_EQ(&mvertex.geom_vertex(), &gvertex);
    EXPECT_EQ(mvertex.global_id(), 0);
    auto & mpars = mvertex.meshing_parameters();
    EXPECT_EQ(mpars.get<int>("marker"), 0);

    mvertex.set_global_id(123);
    EXPECT_EQ(mvertex.global_id(), 123);
}
