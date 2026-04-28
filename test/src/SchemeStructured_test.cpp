#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/scheme/structured.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/bias.h"
#include "krado/exception.h"
#include "builder.h"

using namespace krado;

TEST(SchemeStructuredTest, square_1x1)
{
    auto shape = testing::build_rect(Point(0, 0, 0), Point(1, 1, 0));
    GeomModel model(shape);

    SchemeEqual::Options opts_equal;
    opts_equal.intervals = 1;
    for (int i = 1; i <= 4; ++i) {
        model.curve(i)->set_scheme<SchemeEqual>(opts_equal);
        model.mesh_curve(i);
    }

    SchemeStructured::Options opts_struct;
    model.surface(1)->set_scheme<SchemeStructured>(opts_struct);
    model.mesh_surface(1);

    auto surface = model.surface(1);
    EXPECT_EQ(surface->quadrangles().size(), 1);
    EXPECT_EQ(surface->all_vertices().size(), 4);
}

TEST(SchemeStructuredTest, rect_3x2)
{
    auto shape = testing::build_rect(Point(0, 0, 0), Point(3, 2, 0));
    GeomModel model(shape);

    SchemeEqual::Options opts_h;
    opts_h.intervals = 3;
    SchemeEqual::Options opts_v;
    opts_v.intervals = 2;

    model.curve(1)->set_scheme<SchemeEqual>(opts_h);
    model.curve(2)->set_scheme<SchemeEqual>(opts_v);
    model.curve(3)->set_scheme<SchemeEqual>(opts_h);
    model.curve(4)->set_scheme<SchemeEqual>(opts_v);

    for (int i = 1; i <= 4; ++i)
        model.mesh_curve(i);

    SchemeStructured::Options opts_struct;
    model.surface(1)->set_scheme<SchemeStructured>(opts_struct);
    model.mesh_surface(1);

    auto surface = model.surface(1);
    EXPECT_EQ(surface->quadrangles().size(), 3 * 2);
}

TEST(SchemeStructuredTest, rect_3x2_progressing)
{
    auto shape = testing::build_rect(Point(0, 0, 0), Point(3, 2, 0));
    GeomModel model(shape);

    SchemeBias::Options opts_h1;
    opts_h1.intervals = 3;
    opts_h1.factor = 2.0;
    model.curve(1)->set_scheme<SchemeBias>(opts_h1);
    // We need to match intervals on opposite sides
    SchemeBias::Options opts_h3;
    opts_h3.intervals = 3;
    opts_h3.factor = 0.5;
    model.curve(3)->set_scheme<SchemeBias>(opts_h3);

    SchemeEqual::Options opts_v;
    opts_v.intervals = 2;
    model.curve(2)->set_scheme<SchemeEqual>(opts_v);
    model.curve(4)->set_scheme<SchemeEqual>(opts_v);

    for (int i = 1; i <= 4; ++i)
        model.mesh_curve(i);

    SchemeStructured::Options opts_struct;
    model.surface(1)->set_scheme<SchemeStructured>(opts_struct);
    model.mesh_surface(1);

    auto surface = model.surface(1);
    EXPECT_EQ(surface->quadrangles().size(), 3 * 2);
}

TEST(SchemeStructuredTest, circle_fails)
{
    auto shape = testing::build_circle(Point(0, 0, 0), 1.0);
    GeomModel model(shape);

    // Circle only has 1 curve
    SchemeStructured::Options opts_struct;
    model.surface(1)->set_scheme<SchemeStructured>(opts_struct);

    EXPECT_THROW(model.mesh_surface(1), Exception);
}
