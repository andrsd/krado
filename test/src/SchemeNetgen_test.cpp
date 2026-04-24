#include "gmock/gmock.h"
#include "krado/geom_shape.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme/netgen.h"
#include "krado/scheme/equal.h"
#include "krado/step_file.h"
#include "builder.h"
#include <filesystem>

using namespace krado;
namespace fs = std::filesystem;

TEST(SchemeNetgenTest, line)
{
    auto shape = testing::build_line(Point(0, 0, 0), Point(1, 0, 0));
    GeomModel model(shape);

    SchemeNetgen::Options opts;
    model.curve(1)->set_scheme<SchemeNetgen>(opts);
    model.vertex(1)->set_mesh_size(0.1);
    model.vertex(2)->set_mesh_size(0.6);
    model.mesh_curve(1);

    auto line = model.curve(1);
    auto & all_vtxs = line->all_vertices();

    ASSERT_EQ(all_vtxs.size(), 8);

    EXPECT_NEAR(all_vtxs[0]->point().x, 0.0, 1e-3);
    EXPECT_NEAR(all_vtxs[1]->point().x, 1.0, 1e-3);
    EXPECT_NEAR(all_vtxs[2]->point().x, 0.096, 1e-3);
    EXPECT_NEAR(all_vtxs[3]->point().x, 0.220, 1e-3);
    EXPECT_NEAR(all_vtxs[4]->point().x, 0.376, 1e-3);
    EXPECT_NEAR(all_vtxs[5]->point().x, 0.532, 1e-3);
    EXPECT_NEAR(all_vtxs[6]->point().x, 0.688, 1e-3);
    EXPECT_NEAR(all_vtxs[7]->point().x, 0.844, 1e-3);

    auto & segments = line->segments();
    ASSERT_EQ(segments.size(), 7);
}

TEST(SchemeNetgenTest, circle)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    SchemeNetgen::Options opts;
    model.curve(1)->set_scheme<SchemeNetgen>(opts);
    model.mesh_curve(1);

    auto curv = model.curve(1);

    auto & all_vtxs = curv->all_vertices();
    ASSERT_EQ(all_vtxs.size(), 13);

    // Check key points on the circle
    EXPECT_NEAR(all_vtxs[0]->point().x, 1.0, 1e-6);
    EXPECT_NEAR(all_vtxs[0]->point().y, 0.0, 1e-6);
    EXPECT_NEAR(all_vtxs[3]->point().x, 0.12, 1e-1);
    EXPECT_NEAR(all_vtxs[3]->point().y, 0.99, 1e-1);
    EXPECT_NEAR(all_vtxs[6]->point().x, -0.97, 1e-1);
    EXPECT_NEAR(all_vtxs[6]->point().y, 0.23, 1e-1);

    for (auto & v : all_vtxs) {
        auto p = v->point();
        EXPECT_NEAR(p.distance(Point(0, 0, 0)), 1.0, 1e-6);
    }

    auto & segments = curv->segments();
    ASSERT_EQ(segments.size(), 13);
    for (std::size_t i = 0; i < segments.size(); ++i) {
        EXPECT_EQ(segments[i].vertex(0), all_vtxs[i]);
        EXPECT_EQ(segments[i].vertex(1), all_vtxs[(i + 1) % all_vtxs.size()]);
    }
}

TEST(SchemeNetgenTest, quarter_circle)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quarter-circle.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    SchemeNetgen::Options opts1;
    model.curve(1)->set_scheme<SchemeNetgen>(opts1);
    model.mesh_curve(1);

    auto curv = model.curve(1);

    auto & all_vtxs = curv->all_vertices();
    ASSERT_EQ(all_vtxs.size(), 4);

    EXPECT_NEAR(all_vtxs[0]->point().x, -1., 1e-6);
    EXPECT_NEAR(all_vtxs[0]->point().y, 0., 1e-6);
    EXPECT_NEAR(all_vtxs[1]->point().x, 0., 1e-1);
    EXPECT_NEAR(all_vtxs[1]->point().y, 1., 1e-1);
    EXPECT_NEAR(all_vtxs[2]->point().x, -0.866, 1e-2);
    EXPECT_NEAR(all_vtxs[2]->point().y, 0.50, 1e-1);
    EXPECT_NEAR(all_vtxs[3]->point().x, -0.5, 1e-1);
    EXPECT_NEAR(all_vtxs[3]->point().y, 0.86, 1e-1);

    for (auto & v : all_vtxs) {
        auto p = v->point();
        EXPECT_NEAR(p.distance(Point(0, 0, 0)), 1.0, 1e-6);
    }
}

TEST(SchemeNetgenTest, disc)
{
    auto circle = testing::build_circle(Point(0, 0, 0), 1);
    GeomModel model(circle);

    {
        SchemeEqual::Options opts;
        opts.intervals = 8;
        model.curve(1)->set_scheme<SchemeEqual>(opts);
        model.mesh_curve(1);
    }

    auto surf = model.surface(1);
    EXPECT_TRUE(surf != nullptr);
    EXPECT_EQ(surf->curves().size(), 1);

    {
        SchemeNetgen::Options opts;
        surf->set_scheme<SchemeNetgen>(opts);
        surf->set_mesh_size(0.75);
        model.mesh_surface(1);
    }

    auto & all_verts = surf->all_vertices();
    EXPECT_GT(all_verts.size(), 0);

    auto & triangles = surf->triangles();
    EXPECT_GT(triangles.size(), 0);

    // With mesh size 0.75 and 8 boundary vertices, we expect a fan triangulation
    // with a central point
    EXPECT_EQ(triangles.size(), 7);

    // All triangles should share a common central vertex
    auto central_vertex = triangles[0].vertices()[2];
    for (const auto & tri : triangles) {
        auto verts = tri.vertices();
        // Each triangle should have the central vertex
        bool has_central = (verts[0] == central_vertex || verts[1] == central_vertex ||
                            verts[2] == central_vertex);
        EXPECT_TRUE(has_central);
    }
}
