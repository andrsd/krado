#include "gmock/gmock.h"
#include "builder.h"
#include "krado/point.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_element.h"

using namespace krado;

TEST(MeshElementTest, circum_radius_quality)
{
    auto geom_tri = testing::build_triangle(Point(0, 0, 0), 1.);
    auto gv0 = geom_tri.curves()[0].first_vertex();
    auto gv1 = geom_tri.curves()[1].first_vertex();
    auto gv2 = geom_tri.curves()[2].last_vertex();

    auto v0 = Ptr<MeshVertex>::alloc(1, gv0);
    auto v1 = Ptr<MeshVertex>::alloc(2, gv1);
    auto v2 = Ptr<MeshVertex>::alloc(3, gv2);

    auto tri = MeshElement::Tri3({ v0, v1, v2 });
    auto r = circum_radius_quality(tri);
    EXPECT_NEAR(r, 1.20711, 1e-5);
}

TEST(MeshElementTest, circum_radius_euclidian)
{
    auto geom_tri = testing::build_triangle(Point(0, 0, 0), 1.);
    auto gv0 = geom_tri.curves()[0].first_vertex();
    auto gv1 = geom_tri.curves()[1].first_vertex();
    auto gv2 = geom_tri.curves()[2].last_vertex();

    auto v0 = Ptr<MeshVertex>::alloc(1, gv0);
    auto v1 = Ptr<MeshVertex>::alloc(2, gv1);
    auto v2 = Ptr<MeshVertex>::alloc(3, gv2);

    auto tri = MeshElement::Tri3({ v0, v1, v2 });
    auto r = circum_radius_euclidian(tri, 1.);
    EXPECT_NEAR(r, 0.707107, 1e-6);
}

TEST(MeshElementTest, barycenter)
{
    auto tri_shape = testing::build_triangle(Point(0, 0, 0), 2);

    auto gv0 = tri_shape.curves()[0].first_vertex();
    auto gv1 = tri_shape.curves()[1].first_vertex();
    auto gv2 = tri_shape.curves()[2].last_vertex();

    auto v0 = Ptr<MeshVertex>::alloc(1, gv0);
    auto v1 = Ptr<MeshVertex>::alloc(2, gv1);
    auto v2 = Ptr<MeshVertex>::alloc(3, gv2);

    auto tri = MeshElement::Tri3({ v0, v1, v2 });
    auto bc = barycenter(tri);
    EXPECT_NEAR(bc.x, 2. / 3., 1e-15);
    EXPECT_NEAR(bc.y, 2. / 3., 1e-15);
    EXPECT_NEAR(bc.z, 0., 1e-15);
}
