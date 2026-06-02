#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_volume.h"
#include "krado/geom_model.h"
#include "krado/vector.h"
#include "krado/utils.h"
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
    auto mcurve = Ptr<MeshCurve>::alloc(1, crvs[0], Ptr<MeshVertex>(), Ptr<MeshVertex>());

    std::vector<Ptr<MeshCurve>> c = { mcurve };
    auto msurface = Ptr<MeshSurface>::alloc(1, gsurf, c);
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
    auto triangles = msurface->triangles();
    ASSERT_EQ(triangles.size(), 1);
    EXPECT_EQ(triangles[0].vertex(0), mvtx2);
    EXPECT_EQ(triangles[0].vertex(1), mvtx0);
    EXPECT_EQ(triangles[0].vertex(2), mvtx1);
}

TEST(MeshSurfaceTest, op_shl_circle)
{
    auto circ = testing::build_circle(Point(0, 0, 0), 2.);
    GeomModel model(circ);

    auto surf = model.surface(1);
    std::stringstream ss;
    ss << *surf;
    EXPECT_EQ(ss.str(), "Surface 1: curves=[1], (u, v)=[-2, 2]x[-2, 2], area=12.5664");
}

TEST(MeshSurfaceTest, op_shl_rect)
{
    auto rect = testing::build_rect(Point(1, -1, 0), Point(4, 1.5, 0));
    GeomModel model(rect);

    auto surf = model.surface(1);
    std::stringstream ss;
    ss << *surf;
    EXPECT_EQ(ss.str(),
              "Surface 1: curves=[1, 2, 3, 4], (u, v)=[-1.5, 1.5]x[-1.25, 1.25], area=7.5");
}

TEST(MeshSurfaceTest, quads_to_tris_2)
{
    auto rect = testing::build_rect(Point(0, 0, 0), Point(1, 1, 0));
    GeomModel model(rect);

    auto msurface = model.surface(1);

    auto v0 = model.vertex(1);
    auto v1 = model.vertex(2);
    auto v2 = model.vertex(3);
    auto v3 = model.vertex(4);
    msurface->add_quadrangle(ccw_quadrangle(rect, v0, v1, v2, v3));

    EXPECT_EQ(msurface->surface_vertices().size(), 0);
    EXPECT_EQ(msurface->quadrangles().size(), 1);
    EXPECT_EQ(msurface->triangles().size(), 0);

    msurface->quads_to_tris();
    EXPECT_EQ(msurface->surface_vertices().size(), 0);
    EXPECT_EQ(msurface->quadrangles().size(), 0);
    EXPECT_EQ(msurface->triangles().size(), 2);
}

TEST(MeshSurfaceTest, quads_to_tris_4)
{
    auto rect = testing::build_rect(Point(0, 0, 0), Point(1, 1, 0));
    GeomModel model(rect);

    auto msurface = model.surface(1);

    auto v0 = model.vertex(1);
    auto v1 = model.vertex(2);
    auto v2 = model.vertex(3);
    auto v3 = model.vertex(4);
    msurface->add_quadrangle(ccw_quadrangle(rect, v0, v1, v2, v3));

    EXPECT_EQ(msurface->surface_vertices().size(), 0);
    EXPECT_EQ(msurface->quadrangles().size(), 1);
    EXPECT_EQ(msurface->triangles().size(), 0);

    msurface->quads_to_tris(QuadSplitMode::SPLIT4);
    EXPECT_EQ(msurface->surface_vertices().size(), 1);
    EXPECT_EQ(msurface->quadrangles().size(), 0);
    EXPECT_EQ(msurface->triangles().size(), 4);
}
