#include "gmock/gmock.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/triannular.h"

using namespace krado;

TEST(SchemeTriAnnularTest, decreasing)
{
    auto circle = testing::build_annulus(Point(0, 0, 0), 2., 1.);
    GeomModel model(circle);

    {
        SchemeEqual::Options opts1;
        opts1.intervals = 24;
        model.curve(1)->set_scheme<SchemeEqual>(opts1);
    }
    {
        SchemeEqual::Options opts1;
        opts1.intervals = 12;
        model.curve(2)->set_scheme<SchemeEqual>(opts1);
    }

    auto surf = model.surface(1);
    SchemeTriAnnular::Options opts;
    opts.radial_intervals = 2;
    surf->set_scheme<SchemeTriAnnular>(opts);
    surf->set_marker(1000);
    model.mesh_surface(1);

    auto sv = surf->surface_vertices();
    ASSERT_EQ(sv.size(), 18);

    EXPECT_EQ(surf->triangles().size(), 72);
}
