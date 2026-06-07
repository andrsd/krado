// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmock/gmock.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/scheme/equal.h"
#include "krado/scheme/trifrontal.h"
#include "krado/log.h"

using namespace krado;

TEST(SchemeTriFrontalTest, rectangle)
{
    auto rect = testing::build_rect(Point(0, 0, 0), Point(2, 1, 0));
    GeomModel model(rect);

    for (ShapeID i : { 1, 3 }) {
        auto curve = model.curve(i);
        SchemeEqual::Options opts;
        opts.intervals = 3;
        curve->set_scheme<SchemeEqual>(opts);
    }
    for (ShapeID i : { 2, 4 }) {
        auto curve = model.curve(i);
        SchemeEqual::Options opts;
        opts.intervals = 2;
        curve->set_scheme<SchemeEqual>(opts);
    }

    SchemeTriFrontal::Options opts;
    model.surface(1)->set_mesh_size(1);
    model.surface(1)->set_scheme<SchemeTriFrontal>(opts);
    model.mesh_surface(1);

    auto surf = model.surface(1);
    EXPECT_EQ(surf->surface_vertices().size(), 1);
    auto & sv = surf->surface_vertices()[0];
    EXPECT_TRUE(sv->point().is_equal(Point(1, 0.5, 0.)));

    EXPECT_EQ(surf->triangles().size(), 10);
}

TEST(SchemeTriFrontalTest, circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 0.5);
    GeomModel model(circle);

    {
        auto curve = model.curve(1);
        SchemeEqual::Options opts;
        opts.intervals = 8;
        curve->set_scheme<SchemeEqual>(opts);
    }

    SchemeTriFrontal::Options opts;
    opts.max_size = 0.75;
    model.surface(1)->set_scheme<SchemeTriFrontal>(opts);
    model.surface(1)->set_marker(1000);
    model.surface(1)->set_mesh_size(0.5);
    model.mesh_surface(1);

    auto surf = model.surface(1);
    EXPECT_EQ(surf->surface_vertices().size(), 2);
    auto & sv1 = surf->surface_vertices()[0];
    EXPECT_TRUE(sv1->point().is_equal(Point(0.12059, 0.0499502, 0.), 1e-5));
    auto & sv2 = surf->surface_vertices()[1];
    EXPECT_TRUE(sv2->point().is_equal(Point(-0.181639, -0.0752373, 0.), 1e-5));

    EXPECT_EQ(surf->triangles().size(), 10);
}

TEST(SchemeTriFrontalTest, quarter_circle)
{
    auto qcirc = testing::build_quarter_circle(Point(0, 0, 0), 1);
    GeomModel model(qcirc);

    SchemeTriFrontal::Options opts;
    opts.max_size = 0.5;
    model.surface(1)->set_scheme<SchemeTriFrontal>(opts);
    model.surface(1)->set_marker(1000);
    model.surface(1)->set_mesh_size(0.5);
    model.mesh_surface(1);

    auto surf = model.surface(1);
    EXPECT_EQ(surf->surface_vertices().size(), 1);
    auto & sv = surf->surface_vertices()[0];
    EXPECT_TRUE(sv->point().is_equal(Point(0.433013, 0.433013, 0.), 1e-5));

    EXPECT_EQ(surf->triangles().size(), 7);
}
