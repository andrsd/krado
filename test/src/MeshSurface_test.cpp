#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "builder.h"
#include <array>

using namespace krado;
using namespace testing;

TEST(MeshSurfaceTest, api)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface gsurf(circ);
    auto crvs = gsurf.curves();
    ASSERT_EQ(crvs.size(), 1);
    auto mcurve = Ptr<MeshCurve>::alloc(crvs[0], Ptr<MeshVertex>(), Ptr<MeshVertex>());

    std::vector<Ptr<MeshCurve>> c = { mcurve };
    auto msurface = Ptr<MeshSurface>::alloc(gsurf, c);
    EXPECT_EQ(&msurface->geom_surface(), &gsurf);

    // EXPECT_EQ(msurface->scheme().name(), "auto");

    auto mcs = msurface->curves();
    EXPECT_EQ(mcs.size(), 1);
    EXPECT_EQ(mcs[0], mcurve);

    auto mvtx0 = Ptr<MeshSurfaceVertex>::alloc(gsurf, 0., 0.);
    msurface->add_vertex(mvtx0);
    auto mvtx1 = Ptr<MeshSurfaceVertex>::alloc(gsurf, 0.1, 0.);
    msurface->add_vertex(mvtx1);
    auto mvtx2 = Ptr<MeshSurfaceVertex>::alloc(gsurf, 0., 0.1);
    msurface->add_vertex(mvtx2);

    msurface->add_triangle({ mvtx2, mvtx0, mvtx1 });
    auto & triangles = msurface->triangles();
    ASSERT_EQ(triangles.size(), 1);
    EXPECT_EQ(triangles[0].vertex(0), mvtx2);
    EXPECT_EQ(triangles[0].vertex(1), mvtx0);
    EXPECT_EQ(triangles[0].vertex(2), mvtx1);
}
