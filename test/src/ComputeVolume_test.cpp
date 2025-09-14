#include "gmock/gmock.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve.h"
#include "krado/ops.h"
#include "krado/exodusii_file.h"
#include <filesystem>

using namespace krado;
using namespace testing;
namespace fs = std::filesystem;

TEST(ComputeVolumeTest, length_of_a_line)
{
    auto ln = build_line(Point(0, 0, 0), Point(3, 4, 0));
    GeomModel model(ln);

    // clang-format off
    model.curve(1)
         ->set_scheme("equal")
         .set("intervals", 5);
    // clang-format on
    model.mesh_curve(1);
    auto mesh = model.build_mesh();

    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(5., 1e-10))));
}

TEST(ComputeVolumeTest, area_of_a_square)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "square-half-tri.e");
    auto mesh = f.read();
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(4., 1e-10))));
}

TEST(ComputeVolumeTest, volume_of_a_cube_tet4)
{
    ExodusIIFile f(fs::path(KRADO_UNIT_TESTS_ROOT) / "assets" / "cube-tet.e");
    auto mesh = f.read();
    auto vols = compute_volume(mesh);
    EXPECT_THAT(vols, ElementsAre(Pair(0, DoubleNear(1., 1e-10))));
}
