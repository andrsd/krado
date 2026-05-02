#include "gmock/gmock.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/scheme/trisurf.h"
#include "krado/exodusii_file.h"

using namespace krado;

TEST(SchemeTriSurfTest, cylinder)
{
    auto cyl = testing::build_cylinder(Point(0, 0, 0), 0.75, 1.25);
    GeomModel model(cyl);

    SchemeTriSurf::Options opts;
    opts.is_relative = true;
    opts.linear_deflection = 1.;
    opts.angular_deflection = 1.;
    model.volume(1)->set_scheme<SchemeTriSurf>(opts);
    model.volume(1)->set_marker(100);
    model.mesh_volume(1);

    auto surf1 = model.surface(1);
    auto tris1 = surf1->triangles();
    EXPECT_EQ(tris1.size(), 26);

    auto surf2 = model.surface(2);
    auto tris2 = surf2->triangles();
    EXPECT_EQ(tris2.size(), 11);

    auto surf3 = model.surface(3);
    auto tris3 = surf3->triangles();
    EXPECT_EQ(tris3.size(), 11);
}

TEST(SchemeTriSurfTest, box)
{
    auto cyl = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomModel model(cyl);

    SchemeTriSurf::Options opts;
    opts.is_relative = true;
    opts.linear_deflection = 1.;
    opts.angular_deflection = 1.;
    model.volume(1)->set_scheme<SchemeTriSurf>(opts);
    model.volume(1)->set_marker(100);
    model.mesh_volume(1);
}
