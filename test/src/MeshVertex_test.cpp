#include "gmock/gmock.h"
#include "krado/point.h"
#include "krado/geom_vertex.h"
#include "krado/mesh_vertex.h"
#include "builder.h"

using namespace krado;

TEST(MeshVertexTest, api)
{
    auto vtx = testing::build_vertex(Point(3, 4, 0));
    GeomVertex gvertex(vtx);

    MeshVertex mvertex(gvertex);

    EXPECT_EQ(&mvertex.geom_vertex(), &gvertex);
    EXPECT_EQ(mvertex.global_id(), 0);

    mvertex.set_global_id(123);
    EXPECT_EQ(mvertex.global_id(), 123);
}
