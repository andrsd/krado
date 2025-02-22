#include "gmock/gmock.h"
#include "builder.h"
#include "krado/types.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_element.h"
#include "krado/quality_measures.h"

using namespace krado;

TEST(QualityMeasuresTest, eta_tri3)
{
    auto gtri = testing::build_triangle(Point(0., 0., 0.), 1.);

    auto v1 = gtri.curves()[0].first_vertex();
    auto v2 = gtri.curves()[0].last_vertex();
    auto v3 = gtri.curves()[1].first_vertex();

    MeshVertex mvtx1(v1);
    MeshVertex mvtx2(v2);
    MeshVertex mvtx3(v3);

    auto tri = MeshElement(ElementType::TRI3, { &mvtx1, &mvtx2, &mvtx3 });
    auto eta = quality::eta(tri);
    EXPECT_NEAR(eta, 0.75, 1e-8);
}

TEST(QualityMeasuresTest, eta_quad4)
{
    auto gquad = testing::build_rect(Point(0, 0, 0), Point(3, 1));

    auto v1 = gquad.curves()[0].first_vertex();
    auto v2 = gquad.curves()[1].first_vertex();
    auto v3 = gquad.curves()[2].first_vertex();
    auto v4 = gquad.curves()[3].first_vertex();

    MeshVertex mvtx1(v1);
    MeshVertex mvtx2(v2);
    MeshVertex mvtx3(v3);
    MeshVertex mvtx4(v4);

    auto quad = MeshElement(ElementType::QUAD4, { &mvtx1, &mvtx2, &mvtx3, &mvtx4 });
    auto eta = quality::eta(quad);
    EXPECT_NEAR(eta, 1., 1e-8);
}
