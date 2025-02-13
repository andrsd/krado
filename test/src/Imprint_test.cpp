#include <exception>
#include <gmock/gmock.h>
#include "krado/geom_volume.h"
#include "krado/imprint.h"
#include "krado/geom_surface.h"
#include "krado/geom_curve.h"
#include "krado/geom_shell.h"
#include "builder.h"

using namespace krado;

TEST(ImprintTest, surface_with_curve)
{
    auto circ_face = testing::build_circle(Point(0, 0, 0), 2.);
    GeomSurface circ(circ_face);

    auto line = testing::build_line(Point(2, 2), Point(-2, -2));
    GeomCurve curve(line);

    auto result = imprint(circ, curve);

    auto surfaces = result.surfaces();
    EXPECT_EQ(surfaces.size(), 2);
}

TEST(ImprintTest, volume_with_curve)
{
    auto box_solid = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomVolume box(box_solid);

    auto line = testing::build_line(Point(2, 2, -10), Point(-2, -2, -10));
    GeomCurve curve(line);

    auto result = imprint(box, curve);

    auto surfaces = result.surfaces();
    EXPECT_EQ(surfaces.size(), 8);
}
