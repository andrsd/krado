#include <gmock/gmock.h>
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/fan.h"
#include "krado/point.h"

using namespace krado;

TEST(SchemeFanTest, error_when_more_radial_then_circular_segments)
{
    auto qcirc = testing::build_quarter_circle(Point(0, 0, 0), 3);

    GeomModel model(qcirc);
    {
        SchemeEqual::Options opts;
        opts.intervals = 6;

        for (ShapeID i : { 2, 3 }) {
            model.curve(i)->set_scheme<SchemeEqual>(opts);
            model.mesh_curve(i);
        }
    }
    {
        SchemeEqual::Options opts;
        opts.intervals = 5;
        model.curve(1)->set_scheme<SchemeEqual>(opts);
        model.mesh_curve(1);
    }

    SchemeFan::Options opts;
    model.surface(1)->set_marker(1000);
    model.surface(1)->set_scheme<SchemeFan>(opts);

    EXPECT_THROW(model.mesh_surface(1), Exception);
}

TEST(SchemeFanTest, quarter_circle)
{
    auto qcirc = testing::build_quarter_circle(Point(0, 0, 0), 3);

    GeomModel model(qcirc);
    {
        SchemeEqual::Options opts;
        opts.intervals = 4;

        for (ShapeID i : { 2, 3 }) {
            model.curve(i)->set_scheme<SchemeEqual>(opts);
            model.mesh_curve(i);
        }
    }
    {
        SchemeEqual::Options opts;
        opts.intervals = 5;
        model.curve(1)->set_scheme<SchemeEqual>(opts);
        model.mesh_curve(1);
    }

    {
        SchemeFan::Options opts;
        model.surface(1)->set_marker(1000);
        model.surface(1)->set_scheme<SchemeFan>(opts);
        model.mesh_surface(1);
    }

    EXPECT_EQ(model.surface(1)->triangles().size(), 23);
}
