#include "gmock/gmock.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/scheme.h"
#include "builder.h"

using namespace krado;

TEST(MeshVolumeTest, api)
{
    auto box = testing::build_box(Point(0, 0, 0), Point(2, 3, 4));
    GeomVolume gvol(box);
    auto surfs = gvol.surfaces();
    ASSERT_EQ(surfs.size(), 6);

    std::vector<Ptr<MeshSurface>> msurfs;
    for (auto & gs : surfs) {
        std::vector<Ptr<MeshCurve>> c;
        msurfs.emplace_back(Ptr<MeshSurface>::alloc(gs, c));
    }

    auto mvol = Ptr<MeshVolume>::alloc(gvol, msurfs);
    EXPECT_EQ(&mvol->geom_volume(), &gvol);

    EXPECT_EQ(mvol->scheme().name(), "auto");

    auto mss = mvol->surfaces();
    ASSERT_EQ(mss.size(), 6);
    for (int i = 0; i < 6; i++)
        EXPECT_EQ(mss[i], msurfs[i]);
}
