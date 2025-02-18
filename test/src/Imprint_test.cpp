#include <gmock/gmock.h>
#include "krado/exception.h"
#include "krado/geom_model.h"
#include "krado/geom_volume.h"
#include "krado/ops.h"
#include "krado/geom_surface.h"
#include "krado/geom_curve.h"
#include "krado/geom_shell.h"
#include "builder.h"
#include "ExceptionTestMacros.h"

using namespace krado;

TEST(ImprintTest, surface_with_curve)
{
    auto circ_face = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    auto line = testing::build_line(Point(2, 2, -1), Point(-2, -2, -1));

    auto result = imprint(circ, line);

    auto surfaces = result.surfaces();
    EXPECT_EQ(surfaces.size(), 2);
    EXPECT_NEAR(surfaces[0].area(), M_PI * 2, 1e-10);
    EXPECT_NEAR(surfaces[1].area(), M_PI * 2, 1e-10);
}

TEST(ImprintTest, volume_with_curve)
{
    auto box_solid = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomVolume box(box_solid);

    auto line = testing::build_line(Point(2, 2, 0), Point(-2, -2, 0));

    auto result = imprint(box, line);

    auto surfaces = result.surfaces();
    EXPECT_EQ(surfaces.size(), 7);
}

TEST(ImprintTest, volume_with_curve_that_is_too_far)
{
    auto box_solid = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomVolume box(box_solid);

    auto line = testing::build_line(Point(2, 2, -1), Point(-2, -2, -1));

    EXPECT_THROW_MSG(auto res = imprint(box, line),
                     "Imprint: projection of curve onto volume yield empty result.");
}

TEST(ImprintTest, volume_with_volume)
{
    auto box1 = testing::build_box(Point(0, -1, -0.5), Point(1.5, 1, 2));
    auto box2 = testing::build_box(Point(0, -0.25, 0.5), Point(-0.5, 0.25, 1.5));

    auto result = imprint(box1, box2);

    auto surfaces = result.surfaces();
    EXPECT_EQ(surfaces.size(), 7);
    EXPECT_DOUBLE_EQ(surfaces[0].area(), 4.5);
    EXPECT_DOUBLE_EQ(surfaces[1].area(), 0.5);
    EXPECT_DOUBLE_EQ(surfaces[2].area(), 5.);
    EXPECT_DOUBLE_EQ(surfaces[3].area(), 3.75);
    EXPECT_DOUBLE_EQ(surfaces[4].area(), 3.75);
    EXPECT_DOUBLE_EQ(surfaces[5].area(), 3.);
    EXPECT_DOUBLE_EQ(surfaces[6].area(), 3.);
}
