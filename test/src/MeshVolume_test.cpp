#include "gmock/gmock.h"
#include "krado/geom_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/scheme.h"
#include "builder.h"

using namespace krado;

TEST(MeshVolumeTest, api)
{
    auto box = testing::build_box(Point(0, 0, 0), Point(2, 3, 4));
    GeomVolume gvol(box);
    auto surfs = gvol.surfaces();
    ASSERT_EQ(surfs.size(), 6);

    std::vector<MeshSurface> msurfs;
    for (auto & gs : surfs)
        msurfs.emplace_back(MeshSurface(gs, {}));

    std::vector<MeshSurface *> msurfs_ptrs;
    for (auto & ms : msurfs)
        msurfs_ptrs.push_back(&ms);

    MeshVolume mvol(gvol, msurfs_ptrs);
    EXPECT_EQ(&mvol.geom_volume(), &gvol);

    EXPECT_EQ(mvol.get_scheme().name(), "auto");
    EXPECT_EQ(mvol.get<int>("marker"), 0);

    auto mss = mvol.surfaces();
    ASSERT_EQ(mss.size(), 6);
    for (int i = 0; i < 6; i++)
        EXPECT_EQ(mss[i], &msurfs[i]);
}
