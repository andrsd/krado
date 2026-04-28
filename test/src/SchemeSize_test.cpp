#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
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
    auto & bv = line->bounding_vertices();
    ASSERT_EQ(bv.size(), 2);
    EXPECT_TRUE(bv[0]->point().is_equal(Point(0, 0, 0), 1e-10));
    EXPECT_TRUE(bv[1]->point().is_equal(Point(1, 0, 0), 1e-10));

    auto & cv = line->curve_vertices();
    ASSERT_EQ(cv.size(), 3);
    EXPECT_TRUE(cv[0]->point().is_equal(Point(0.3, 0, 0), 1e-8));
    EXPECT_TRUE(cv[1]->point().is_equal(Point(0.57, 0, 0), 1e-8));
    EXPECT_TRUE(cv[2]->point().is_equal(Point(0.813, 0, 0), 1e-8));

    ASSERT_EQ(line->segments().size(), 4);
}
