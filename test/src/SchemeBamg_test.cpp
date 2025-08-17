#include "gmock/gmock.h"
#include "krado/config.h"
#include "krado/step_file.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "ExceptionTestMacros.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(SchemeBamgTest, DISABLED_mesh_quad)
{
    fs::path input_file = fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "quad.step";
    STEPFile file(input_file.string());
    auto shapes = file.load();
    auto shape = shapes[0];
    GeomModel model(shape);

    // clang-format off
    model.surface(1)
        ->set("marker", 10)
        .set_scheme("bamg")
        .set("max_area", 0.9);
    model.mesh_surface(1);
    // clang-format on

    auto quad = model.surface(1);
    ASSERT_EQ(quad->all_vertices().size(), 10);
    auto & vtx = quad->all_vertices();
    EXPECT_EQ(vtx[0]->point(), Point(0., 0., 0.));
    EXPECT_EQ(vtx[1]->point(), Point(2., 0., 0.));
    EXPECT_EQ(vtx[2]->point(), Point(2., 3., 0.));
    EXPECT_EQ(vtx[3]->point(), Point(0., 1., 0.));
    EXPECT_EQ(vtx[4]->point(), Point(2., 1., 0.));
    EXPECT_EQ(vtx[5]->point(), Point(2., 2., 0.));
    EXPECT_EQ(vtx[6]->point(), Point(1.3333333333333333, 2.3333333333333333, 0.));
    EXPECT_EQ(vtx[7]->point(), Point(0.6666666666666666, 1.6666666666666666, 0.));
    EXPECT_EQ(vtx[8]->point(), Point(1., 0.4999999984633178, 0.));
    EXPECT_EQ(vtx[9]->point(), Point(1.333333333333333, 1.333333331796651, 0.));
    EXPECT_EQ(quad->triangles().size(), 10);

    auto & tris = quad->triangles();
    // EXPECT_THAT(tris[0].ids(), ElementsAre(8, 7, 3));
    // EXPECT_THAT(tris[1].ids(), ElementsAre(9, 5, 6));
    // EXPECT_THAT(tris[2].ids(), ElementsAre(2, 6, 5));
    // EXPECT_THAT(tris[3].ids(), ElementsAre(8, 9, 7));
    // EXPECT_THAT(tris[4].ids(), ElementsAre(6, 7, 9));
    // EXPECT_THAT(tris[5].ids(), ElementsAre(8, 4, 9));
    // EXPECT_THAT(tris[6].ids(), ElementsAre(0, 8, 3));
    // EXPECT_THAT(tris[7].ids(), ElementsAre(4, 5, 9));
    // EXPECT_THAT(tris[8].ids(), ElementsAre(1, 8, 0));
    // EXPECT_THAT(tris[9].ids(), ElementsAre(1, 4, 8));
}
