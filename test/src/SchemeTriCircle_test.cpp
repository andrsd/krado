#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "ExceptionTestMacros.h"
#include "builder.h"

using namespace krado;

TEST(SchemeCircleTest, DISABLED_circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    // clang-format off
    model.curve(1)
        .set_scheme("equal")
        .set("intervals", 8)
    ;
    // clang-format on
    model.mesh_curve(1);

    auto & surf = model.surface(1);
    // clang-format off
    surf.set_scheme("tricircle")
    ;
    // clang-format on
    model.mesh_surface(1);

    auto SQRT2_2 = std::sqrt(2.) / 2.;

    auto & all_vtxs = surf.all_vertices();
    EXPECT_EQ(all_vtxs[0]->point(), Point(0, 0, 0));
    EXPECT_EQ(all_vtxs[1]->point(), Point(1., 0, 0));
    EXPECT_EQ(all_vtxs[2]->point(), Point(SQRT2_2, SQRT2_2, 0));
    EXPECT_EQ(all_vtxs[3]->point(), Point(0, 1., 0));
    EXPECT_EQ(all_vtxs[4]->point(), Point(-SQRT2_2, SQRT2_2, 0));
    EXPECT_EQ(all_vtxs[5]->point(), Point(-1., 0, 0));
    EXPECT_EQ(all_vtxs[6]->point(), Point(-SQRT2_2, -SQRT2_2, 0));
    EXPECT_EQ(all_vtxs[7]->point(), Point(0, -1., 0));

    EXPECT_EQ(surf.triangles().size(), 8);
}

TEST(SchemeCircleTest, DISABLED_not_circle)
{
    auto circle = testing::build_rect(Point(0, 0, 0), Point(1, 2, 3));
    GeomModel model(circle);

    auto & surf = model.surface(1);
    surf.set_scheme("tricircle");
    EXPECT_THROW_MSG(model.mesh_surface(1), "Surface 1 is not a circle");
}
