// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "builder.h"
#include "krado/scheme/tridelaunay.h"
#include "krado/step_file.h"
#include "krado/log.h"
#include "krado/exodusii_file.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(SchemeTriDelaunayTest, rectangle)
{
    Log::set_verbosity(9);

    auto rect = testing::build_rect(Point(0, 0, 0), Point(1, 1, 0));
    GeomModel model(rect);

    SchemeTriDelaunay::Options opts;
    opts.max_size = 0.1;
    model.surface(1)->set_mesh_size(0.05);
    model.surface(1)->set_scheme<SchemeTriDelaunay>(opts);
    model.mesh_surface(1);

    auto surf = model.surface(1);
    // std::cerr << "tris = " << surf->triangles().size() << std::endl;
    // EXPECT_GT(surf->all_vertices().size(), 4);
    // EXPECT_GT(surf->triangles().size(), 2);
    // for (auto & t : surf->triangles()) {
    //     std::cerr << "- " << t.vertex(0)->point() << ", " << t.vertex(1)->point() << ", "
    //               << t.vertex(2)->point() << std::endl;
    // }

    // debug
    auto mesh = build_mesh(model);
    ExodusIIFile exo("rect.exo");
    exo.write(mesh);
}

TEST(SchemeTriDelaunayTest, circle)
{
    Log::set_verbosity(9);
    auto circle = testing::build_circle(Point(0, 0, 0), 0.5);
    GeomModel model(circle);

    SchemeTriDelaunay::Options opts;
    opts.max_size = 0.5;
    model.surface(1)->set_mesh_size(0.05);
    model.surface(1)->set_scheme<SchemeTriDelaunay>(opts);
    model.mesh_surface(1);

    // auto surf = model.surface(1);
    // EXPECT_GT(surf->all_vertices().size(), 8);
    // EXPECT_GT(surf->triangles().size(), 8);

    // debug
    auto mesh = build_mesh(model);
    ExodusIIFile exo("circle.exo");
    exo.write(mesh);
}

TEST(SchemeTriDelaunayTest, quarter_circle)
{
    Log::set_verbosity(9);

    fs::path input_file =
        fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "geo" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    SchemeTriDelaunay::Options opts;
    opts.max_size = 0.1;
    model.surface(1)->set_mesh_size(0.05);
    model.surface(1)->set_scheme<SchemeTriDelaunay>(opts);
    model.mesh_surface(1);

    // auto surf = model.surface(1);
    // EXPECT_GT(surf->all_vertices().size(), 3);
    // EXPECT_GT(surf->triangles().size(), 1);

    // debug
    auto mesh = build_mesh(model);
    ExodusIIFile exo("qcirc.exo");
    exo.write(mesh);
}
