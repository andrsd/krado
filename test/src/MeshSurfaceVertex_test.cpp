#include "gmock/gmock.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/geom_surface.h"
#include "builder.h"

using namespace krado;

TEST(MeshSurfaceVertexTest, test)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);

    auto uv = gsurf.parameter_from_point(Point(0.5, 1., 0.));

    MeshSurfaceVertex msvtx(gsurf, uv);

    EXPECT_EQ(&msvtx.geom_surface(), &gsurf);

    auto pt = msvtx.point();
    EXPECT_EQ(pt, Point(0.5, 1., 0.));

    auto vtx = msvtx.parameter();
    EXPECT_DOUBLE_EQ(vtx.u, uv.u);
    EXPECT_DOUBLE_EQ(vtx.v, uv.v);
}
