#include "gmock/gmock.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/geom_surface.h"
#include "builder.h"

using namespace krado;

TEST(MeshSurfaceVertexTest, test)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);

    auto [u, v] = gsurf.parameter_from_point(Point(0.5, 1., 0.));

    MeshSurfaceVertex msvtx(gsurf, u, v);

    EXPECT_EQ(&msvtx.geom_surface(), &gsurf);

    auto pt = msvtx.point();
    EXPECT_DOUBLE_EQ(pt.x, 0.5);
    EXPECT_DOUBLE_EQ(pt.y, 1.);
    EXPECT_DOUBLE_EQ(pt.z, 0.);

    auto [vtx_u, vtx_v] = msvtx.parameter();
    EXPECT_DOUBLE_EQ(vtx_u, u);
    EXPECT_DOUBLE_EQ(vtx_v, v);
}
