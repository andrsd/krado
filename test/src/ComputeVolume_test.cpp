#include "gmock/gmock.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/ops.h"
#include "krado/exodusii_file.h"
#include "krado/scheme/equal.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(ComputeVolumeTest, DISABLED_length_of_a_line)
{
    auto ln = build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomModel model(ln);

    SchemeEqual::Options opts;
    opts.intervals = 5;
    model.curve(1)->set_scheme<SchemeEqual>(opts);

    model.mesh_curve(1);
    // FIXME
    // auto mesh = build_mesh(model);
    //
    // auto vols = compute_volume(mesh);
    // EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(5., 1e-10))));
}

TEST(ComputeVolumeTest, area_of_a_square)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "mesh" / "square-half-tri.e");
    auto mesh = f.read();
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(4., 1e-10))));
}

TEST(ComputeVolumeTest, volume_of_a_cube_tet4)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "mesh" / "cube-tet.e");
    auto mesh = f.read();
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(1., 1e-10))));
}
