#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/log.h"
#include "builder.h"

using namespace krado;

TEST(SchemeEqualTest, line)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    // clang-format off
    model.curve(1)
         ->set_scheme("equal")
         .set("intervals", 5);
    // clang-format on
    model.mesh_curve(1);

    auto line = model.curve(1);
    ASSERT_EQ(line->all_vertices().size(), 6);
    auto first_vtx = line->all_vertices().front();
    auto last_vtx = line->all_vertices().back();
    EXPECT_NE(first_vtx, last_vtx);

    ASSERT_EQ(line->curve_vertices().size(), 4);
    auto & cv = line->curve_vertices();
    EXPECT_DOUBLE_EQ(cv[0]->point().x, 0.2);
    EXPECT_DOUBLE_EQ(cv[1]->point().x, 0.4);
    EXPECT_DOUBLE_EQ(cv[2]->point().x, 0.6);
    EXPECT_DOUBLE_EQ(cv[3]->point().x, 0.8);

    ASSERT_EQ(line->segments().size(), 5);
}

TEST(SchemeEqualTest, circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    // clang-format off
    model.curve(1)
        ->set_scheme("equal")
        .set("intervals", 8)
    ;
    // clang-format on
    model.mesh_curve(1);

    auto curv = model.curve(1);

    auto SQRT2_2 = std::sqrt(2.) / 2.;

    auto & all_vtxs = curv->all_vertices();
    ASSERT_EQ(all_vtxs.size(), 8);
    EXPECT_EQ(all_vtxs[0]->point(), Point(1., 0, 0));
    EXPECT_EQ(all_vtxs[1]->point(), Point(SQRT2_2, SQRT2_2, 0));
    EXPECT_EQ(all_vtxs[2]->point(), Point(0, 1., 0));
    EXPECT_EQ(all_vtxs[3]->point(), Point(-SQRT2_2, SQRT2_2, 0));
    EXPECT_EQ(all_vtxs[4]->point(), Point(-1., 0, 0));
    EXPECT_EQ(all_vtxs[5]->point(), Point(-SQRT2_2, -SQRT2_2, 0));
    EXPECT_EQ(all_vtxs[6]->point(), Point(0, -1., 0));
    EXPECT_EQ(all_vtxs[7]->point(), Point(SQRT2_2, -SQRT2_2, 0));

    EXPECT_EQ(curv->segments().size(), 8);
}
