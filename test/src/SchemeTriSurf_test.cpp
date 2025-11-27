#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "builder.h"
#include "krado/scheme/trisurf.h"

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
    model.mesh_volume(1);

    auto surf1 = model.surface(1);
    auto vtxs1 = surf1->all_vertices();
    EXPECT_EQ(vtxs1.size(), 28);

    auto surf2 = model.surface(2);
    auto vtxs2 = surf2->all_vertices();
    EXPECT_EQ(vtxs2.size(), 13);

    auto surf3 = model.surface(3);
    auto vtxs3 = surf3->all_vertices();
    EXPECT_EQ(vtxs3.size(), 13);
}
