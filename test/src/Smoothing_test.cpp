#include "gmock/gmock.h"
#include "builder.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/line.h"
#include "krado/ops.h"
#include "krado/ptr.h"

using namespace krado;
using namespace testing;

TEST(SmoothingTest, relocate_corner)
{
    auto gv = build_vertex(Point(1, 2, 3));
    MeshVertex mv(1, gv);

    EXPECT_THAT(mv.point().x, DoubleEq(1.0));

    mv.relocate(Point(10, 10, 10));
    // Corner should not move
    EXPECT_THAT(mv.point().x, DoubleEq(1.0));
    EXPECT_THAT(mv.point().y, DoubleEq(2.0));
    EXPECT_THAT(mv.point().z, DoubleEq(3.0));
}

TEST(SmoothingTest, relocate_curve)
{
    auto line = build_line(Point(0, 0, 0), Point(10, 0, 0));
    MeshCurveVertex mcv(line, 5.0); // Midpoint

    EXPECT_THAT(mcv.point().x, DoubleEq(5.0));

    // Relocate to a point slightly off the line
    mcv.relocate(Point(6.0, 1.0, 1.0));

    // Should snap back to the line (x=6, y=0, z=0)
    EXPECT_THAT(mcv.point().x, DoubleEq(6.0));
    EXPECT_THAT(mcv.point().y, DoubleEq(0.0));
    EXPECT_THAT(mcv.point().z, DoubleEq(0.0));
    EXPECT_THAT(mcv.parameter(), DoubleEq(6.0));
}

TEST(SmoothingTest, relocate_surface)
{
    auto rect = build_rect(Point(0, 0, 0), Point(10, 10, 0));
    MeshSurfaceVertex msv(rect, 0.0, 0.0);

    EXPECT_THAT(msv.point().x, DoubleEq(5.0));
    EXPECT_THAT(msv.point().y, DoubleEq(5.0));
    EXPECT_THAT(msv.point().z, DoubleEq(0.0));

    // Relocate off the surface
    msv.relocate(Point(6.0, 7.0, 1.0));

    // Should snap back to z=0
    EXPECT_THAT(msv.point().x, DoubleEq(6.0));
    EXPECT_THAT(msv.point().y, DoubleEq(7.0));
    EXPECT_THAT(msv.point().z, DoubleEq(0.0));
    EXPECT_THAT(msv.parameter().u, DoubleEq(1.));
    EXPECT_THAT(msv.parameter().v, DoubleEq(2.));
}

TEST(SmoothingTest, smooth_surface)
{
    auto rect = build_rect(Point(0., 0., 0.), Point(3., 2., 0.));

    auto curves = rect.curves();
    GeomVertex gv1(curves[3].last_vertex());
    GeomVertex gv2(curves[0].last_vertex());
    GeomVertex gv3(curves[2].last_vertex());
    GeomVertex gv4(curves[1].last_vertex());

    // Create a 2x2 grid of vertices
    // V6 V7 V8
    // V3 V4 V5
    // V0 V1 V2

    auto v0 = Ptr<MeshVertex>::alloc(1, gv1); // (0,0,0)
    auto v1 = Ptr<MeshCurveVertex>::alloc(curves[0], 1.0); // (1,0,0)
    auto v2 = Ptr<MeshVertex>::alloc(2, gv2); // (3,0,0)

    auto v3 = Ptr<MeshCurveVertex>::alloc(curves[3], 1.0); // (0,1,0)
    auto v4 = Ptr<MeshSurfaceVertex>::alloc(rect, 0.2, 0.2); // Distorted from (1.5,1)
    auto v5 = Ptr<MeshCurveVertex>::alloc(curves[1], 1.0); // (3,1,0)

    auto v6 = Ptr<MeshVertex>::alloc(3, gv3); // (0,2,0)
    auto v7 = Ptr<MeshCurveVertex>::alloc(curves[2], 1.0); // (2,2,0)
    auto v8 = Ptr<MeshVertex>::alloc(4, gv4); // (3,2,0)

    std::vector<Ptr<MeshCurve>> mcrvs;
    auto surface = Ptr<MeshSurface>::alloc(1, rect, mcrvs);
    surface->add_vertex(v4);

    // Triangles for the 4 quads
    surface->add_triangle({ v0, v1, v4 });
    surface->add_triangle({ v0, v4, v3 });

    surface->add_triangle({ v1, v2, v4 });
    surface->add_triangle({ v2, v5, v4 });

    surface->add_triangle({ v3, v4, v6 });
    surface->add_triangle({ v4, v7, v6 });

    surface->add_triangle({ v4, v5, v8 });
    surface->add_triangle({ v4, v8, v7 });

    // Initial position of internal vertex is (1.7, 1.2, 0)
    EXPECT_THAT(v4->point().x, DoubleEq(1.7));
    EXPECT_THAT(v4->point().y, DoubleEq(1.2));

    // Smooth
    krado::smooth(surface, 2);

    // New position should be average of neighbors (1,1,0)
    EXPECT_NEAR(v4->point().x, 1.5, 1e-7);
    EXPECT_NEAR(v4->point().y, 1.0, 1e-7);
    EXPECT_THAT(v4->point().z, DoubleEq(0.0));
}
