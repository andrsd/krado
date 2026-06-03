#include "gmock/gmock.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/quadannular.h"

using namespace krado;

TEST(SchemeQuadAnnularTest, test_one_radial)
{
    auto circle = testing::build_annulus(Point(0, 0, 0), 2., 1.);
    GeomModel model(circle);

    {
        SchemeEqual::Options opts1;
        opts1.intervals = 8;
        model.curve(1)->set_scheme<SchemeEqual>(opts1);
        model.curve(2)->set_scheme<SchemeEqual>(opts1);
    }

    auto surf = model.surface(1);
    SchemeQuadAnnular::Options opts;
    opts.radial_intervals = 1;
    surf->set_scheme<SchemeQuadAnnular>(opts);
    surf->set_marker(1000);
    model.mesh_surface(1);

    auto sv = surf->surface_vertices();
    ASSERT_EQ(sv.size(), 0);

    EXPECT_EQ(surf->quadrangles().size(), 8);
}
