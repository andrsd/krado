#include "gmock/gmock.h"
#include "krado/geom_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "BRepPrimAPI_MakeBox.hxx"
#include "TopoDS_Solid.hxx"

using namespace krado;

namespace {

TopoDS_Solid
build_box(const Point & v1, const Point & v2)
{
    gp_Pnt pnt1(v1.x, v1.y, v1.z);
    gp_Pnt pnt2(v2.x, v2.y, v2.z);
    BRepPrimAPI_MakeBox make_box(pnt1, pnt2);
    make_box.Build();
    return make_box.Solid();
}

} // namespace

TEST(MeshVolumeTest, api)
{
    auto box = build_box(Point(0, 0, 0), Point(2, 3, 4));
    GeomVolume gvol(box);
    auto surfs = gvol.surfaces();
    ASSERT_EQ(surfs.size(), 6);

    std::vector<MeshSurface> msurfs;
    for (auto & gs : surfs)
        msurfs.emplace_back(MeshSurface(gs, {}));

    std::vector<const MeshSurface *> msurfs_ptrs;
    for (auto & ms : msurfs)
        msurfs_ptrs.push_back(&ms);

    MeshVolume mvol(gvol, msurfs_ptrs);
    EXPECT_EQ(&mvol.geom_volume(), &gvol);

    EXPECT_EQ(mvol.marker(), 0);

    mvol.set_marker(123);
    EXPECT_EQ(mvol.marker(), 123);

    auto mss = mvol.surfaces();
    ASSERT_EQ(mss.size(), 6);
    for (int i = 0; i < 6; i++)
        EXPECT_EQ(mss[i], &msurfs[i]);
}
