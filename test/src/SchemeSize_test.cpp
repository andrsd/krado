#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "builder.h"
#include "krado/scheme/size.h"

using namespace krado;

TEST(SchemeSizeTest, line_with_vertex_sizes)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    model.vertex(1)->set_mesh_size(0.3);
    model.vertex(2)->set_mesh_size(0.2);

    SchemeSize::Options opts;
    model.curve(1)->set_scheme<SchemeSize>(opts);
    model.mesh_curve(1);

    auto line = model.curve(1);
    ASSERT_EQ(line->all_vertices().size(), 5);

    auto & cv = line->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_NEAR(cv[0]->point().x, 0.3, 1e-8);
    EXPECT_NEAR(cv[1]->point().x, 0.57, 1e-8);
    EXPECT_NEAR(cv[2]->point().x, 0.813, 1e-8);

    ASSERT_EQ(line->segments().size(), 4);
}
