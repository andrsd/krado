#include "gmock/gmock.h"
#include "krado/geom_volume.h"
#include "BRepPrimAPI_MakeBox.hxx"
#include "TopoDS_Solid.hxx"
#include "builder.h"

using namespace krado;

TEST(GeomVolumeTest, volume)
{
    auto box_solid = testing::build_box(Point(0, 0, 0), Point(1, 2, 3));
    GeomVolume box(box_solid);

    EXPECT_DOUBLE_EQ(box.volume(), 6.);
}

TEST(GeomVolumeTest, surfaces)
{
    auto box_solid = testing::build_box(Point(0, 0, 0), Point(2, 3, 4));
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
