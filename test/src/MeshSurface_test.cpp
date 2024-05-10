#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme_equal.h"
#include "builder.h"
#include <array>

using namespace krado;

TEST(MeshSurfaceTest, api)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);
    auto crvs = gsurf.curves();
    ASSERT_EQ(crvs.size(), 1);
    MeshCurve mcurve(crvs[0], nullptr, nullptr);

    MeshSurface msurface(gsurf, { &mcurve });
    EXPECT_EQ(&msurface.geom_surface(), &gsurf);

    EXPECT_EQ(msurface.get_scheme().name(), "auto");
    EXPECT_EQ(msurface.get<int>("marker"), 0);

    auto mcs = msurface.curves();
    EXPECT_EQ(mcs.size(), 1);
    EXPECT_EQ(mcs[0], &mcurve);

    auto mvtx0 = new MeshSurfaceVertex(gsurf, 0., 0.);
    msurface.add_vertex(mvtx0);
    auto mvtx1 = new MeshSurfaceVertex(gsurf, 0.1, 0.);
    msurface.add_vertex(mvtx1);
    auto mvtx2 = new MeshSurfaceVertex(gsurf, 0., 0.1);
    msurface.add_vertex(mvtx2);

    msurface.add_triangle({ 2, 0, 1 });
    auto & triangles = msurface.triangles();
    ASSERT_EQ(triangles.size(), 1);
    auto & tri0 = triangles[0];
    EXPECT_EQ(tri0(0), 2);
    EXPECT_EQ(tri0(1), 0);
    EXPECT_EQ(tri0(2), 1);
}
