#include "gmock/gmock.h"
#include "builder.h"
#include "ExceptionTestMacros.h"
#include "krado/geom_curve.h"
#include "krado/geom_model.h"

using namespace krado;

TEST(SchemeTest, mesh_curve_with_2d_scheme)
{
    auto line = testing::build_line(Point(0, 0), Point(1, 0));
    GeomModel model(line);

    // clang-format off
    model.curve(1)
        .set_scheme("bamg");
    // clang-format on
    EXPECT_THROW_MSG(model.mesh_curve(1), "Scheme 'bamg' is not a 1D scheme");
}

TEST(SchemeTest, mesh_surface_with_1d_scheme)
{
    auto rect = testing::build_rect(Point(0, 0), Point(1, 0.5));
    GeomModel model(rect);

    // clang-format off
    model.surface(1)
        .set_scheme("equal");
    // clang-format on
    EXPECT_THROW_MSG(model.mesh_surface(1), "Scheme 'equal' is not a 2D scheme");
}

TEST(SchemeTest, mesh_volume_with_1d_scheme)
{
    auto box = testing::build_box(Point(0., 0., 0.), Point(1., 0.5, 1.25));
    GeomModel model(box);

    // clang-format off
    model.volume(1)
        .set_scheme("bamg");
    // clang-format on
    EXPECT_THROW_MSG(model.mesh_volume(1), "Scheme 'bamg' is not a 3D scheme");
}
