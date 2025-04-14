#include "gmock/gmock.h"
#include "ExceptionTestMacros.h"
#include "builder.h"
#include "krado/geom_model.h"
#include "krado/dagmc_file.h"

using namespace krado;
using namespace testing;

#ifdef KRADO_WITH_MOAB

TEST(DAGMCFileTest, write)
{
    auto shape = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    shape.set_material("steel");
    GeomModel model(shape);

    // clang-format off
    model.volume(1)
         .set_scheme("trisurf")
         .set<bool>("is_relative", true)
         .set<double>("linear_deflection", 1.)
         .set<double>("angular_deflection", 1.)
    ;
    // clang-format on
    model.mesh_volume(1);

    DAGMCFile dagmc("dagmc.h5");
    dagmc.write(model);
}

#else

TEST(DAGMCFileTest, test)
{
    EXPECT_THROW({ DAGMCFile dagmc("dagmc.h5"); }, krado::Exception);
}

#endif
