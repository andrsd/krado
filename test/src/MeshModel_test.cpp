#include "gmock/gmock.h"
#include "krado/geom_model.h"
#include "krado/mesh_model.h"
#include "builder.h"

using namespace krado;
using namespace testing;

TEST(MeshModelTest, ctor)
{
    auto shape = GeomShape(testing::build_box(Point(0, 0, 0), Point(1, 1, 1)));
    GeomModel model(shape);
    MeshModel mesh(model);
    EXPECT_EQ(mesh.vertices().size(), 8);
    EXPECT_EQ(mesh.curves().size(), 12);
    EXPECT_EQ(mesh.surfaces().size(), 6);
    EXPECT_EQ(mesh.volumes().size(), 1);

    auto box = mesh.volume(1);
    EXPECT_EQ(box.surfaces().size(), 6);
}

TEST(MeshTest, circle)
{
    auto shape = GeomShape(testing::build_circle(Point(0, 0, 0), 2.));
    GeomModel model(shape);
    MeshModel mesh(model);
    EXPECT_EQ(mesh.vertices().size(), 1);
    EXPECT_EQ(mesh.curves().size(), 1);
    EXPECT_EQ(mesh.surfaces().size(), 1);
}
