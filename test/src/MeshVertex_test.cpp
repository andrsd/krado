#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/geom_vertex.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/geom_model.h"
#include "builder.h"

using namespace krado;

TEST(MeshVertexTest, api)
{
    auto vtx = testing::build_vertex(Point(3, 4, 0));
    GeomVertex gvertex(vtx);

    MeshVertex mvertex(1, gvertex);

    EXPECT_EQ(&mvertex.geom_vertex(), &gvertex);
}

TEST(MeshVertexTest, op_shl)
{
    auto vtx = testing::build_vertex(Point(1, 2, 3));
    GeomModel model(vtx);

    auto mvtx = model.vertex(1);
    std::stringstream ss;
    ss << *mvtx;
    EXPECT_EQ(ss.str(), "Vertex 1: location=(x=1, y=2, z=3)");
}
