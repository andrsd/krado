#include "gmock/gmock.h"
#include "krado/geom_volume.h"
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

TEST(GeomVolumeTest, volume)
{
    auto box_solid = build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomVolume box(box_solid);

    EXPECT_DOUBLE_EQ(box.volume(), 6.);
}

TEST(GeomVolumeTest, surfaces)
{
    auto box_solid = build_box(Point(0, 0, 0), Point(2, 3, 4));
    GeomVolume box(box_solid);

    auto surfs = box.surfaces();
    EXPECT_EQ(surfs.size(), 6);
    EXPECT_DOUBLE_EQ(surfs[0].area(), 12.);
    EXPECT_DOUBLE_EQ(surfs[1].area(), 12.);
    EXPECT_DOUBLE_EQ(surfs[2].area(), 8.);
    EXPECT_DOUBLE_EQ(surfs[3].area(), 8.);
    EXPECT_DOUBLE_EQ(surfs[4].area(), 6.);
    EXPECT_DOUBLE_EQ(surfs[5].area(), 6.);
}
