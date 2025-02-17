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
    GeomCurve curve(line);

    auto result = imprint(circ, curve);

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
    GeomCurve curve(line);

    auto result = imprint(box, curve);

    auto surfaces = result.surfaces();
    EXPECT_EQ(surfaces.size(), 7);
}

TEST(ImprintTest, volume_with_curve_that_is_too_far)
{
    auto box_solid = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomVolume box(box_solid);

    auto line = testing::build_line(Point(2, 2, -1), Point(-2, -2, -1));
    GeomCurve curve(line);

    EXPECT_THROW_MSG(auto res = imprint(box, curve),
                     "Imprint: projection of curve onto volume yield empty result.");
}
