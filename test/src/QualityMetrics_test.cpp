// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"
#include "krado/quality_measures.h"
#include "krado/mesh.h"
#include "krado/element.h"
#include "krado/point.h"
#include "krado/log.h"

using namespace krado;

TEST(QualityMetricsTest, tri3_aspect_ratio)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(1, 0, 0), Point(0.5, std::sqrt(3) / 2, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }) };
    Mesh mesh(points, elements);

    double ar = qm::aspect_ratio<ElementType::TRI3>(mesh.element(0), mesh);
    EXPECT_NEAR(ar, 1.0, 1e-12);
}

TEST(QualityMetricsTest, tri3_scaled_jacobian)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }) };
    Mesh mesh(points, elements);

    double sj = qm::scaled_jacobian<ElementType::TRI3>(mesh.element(0), mesh);
    EXPECT_NEAR(sj, 1.0, 1e-12);
}

TEST(QualityMetricsTest, tri3_eta)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(1, 0, 0), Point(0.5, std::sqrt(3) / 2, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }) };
    Mesh mesh(points, elements);

    double eta = qm::eta<ElementType::TRI3>(mesh.element(0), mesh);
    EXPECT_NEAR(eta, 1.0, 1e-12);
}

TEST(QualityMetricsTest, tri3_gamma)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(1, 0, 0), Point(0.5, std::sqrt(3) / 2, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }) };
    Mesh mesh(points, elements);

    double gamma = qm::gamma<ElementType::TRI3>(mesh.element(0), mesh);
    EXPECT_NEAR(gamma, 1.0, 1e-12);
}

TEST(QualityMetricsTest, tetra4_gamma)
{
    // Regular tetrahedron
    std::vector<Point> points = { Point(1, 1, 1),
                                  Point(1, -1, -1),
                                  Point(-1, 1, -1),
                                  Point(-1, -1, 1) };
    std::vector<Element> elements = { Element::Tetra4({ 0, 1, 2, 3 }) };
    Mesh mesh(points, elements);

    double gamma = qm::gamma<ElementType::TETRA4>(mesh.element(0), mesh);
    EXPECT_NEAR(gamma, 1.0, 1e-12);
}

TEST(QualityMetricsTest, quad4_aspect_ratio)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(2, 0, 0), Point(2, 1, 0), Point(0, 1, 0) };
    std::vector<Element> elements = { Element::Quad4({ 0, 1, 2, 3 }) };
    Mesh mesh(points, elements);

    double ar = qm::aspect_ratio<ElementType::QUAD4>(mesh.element(0), mesh);
    EXPECT_NEAR(ar, 2.0, 1e-12);
}

TEST(QualityMetricsTest, quad4_skewness_1)
{
    std::vector<Point> points = {
        Point(0, 0, 0),
        Point(1, 0, 0),
        Point(1, 1, 0),
        Point(0, 1, 0),
    };
    std::vector<Element> elements = { Element::Quad4({ 0, 1, 2, 3 }) };
    Mesh mesh(points, elements);

    double sk = qm::skewness<ElementType::QUAD4>(mesh.element(0), mesh);
    EXPECT_NEAR(sk, 0., 1e-12);
}

TEST(QualityMetricsTest, quad4_skewness_2)
{
    std::vector<Point> points = {
        Point(0, 0, 0),
        Point(1, 0, 0),
        Point(1.5, 1, 0),
        Point(0.5, 1, 0),
    };
    std::vector<Element> elements = { Element::Quad4({ 0, 1, 2, 3 }) };
    Mesh mesh(points, elements);

    double sk = qm::skewness<ElementType::QUAD4>(mesh.element(0), mesh);
    EXPECT_NEAR(sk, 0.2951672353, 1e-10);
}

TEST(QualityMetricsTest, hex8_skewness_1)
{
    std::vector<Point> points = {
        Point(0, 0, 0),    Point(1, 0, 0),    Point(1.5, 1, 0),  Point(0.5, 1, 0),
        Point(0.25, 0, 2), Point(1.25, 0, 2), Point(1.75, 1, 2), Point(0.75, 1, 2),
    };
    std::vector<Element> elements = { Element::Hex8({ 0, 1, 2, 3, 4, 5, 6, 7 }) };
    Mesh mesh(points, elements);

    double sk = qm::skewness<ElementType::HEX8>(mesh.element(0), mesh);
    EXPECT_NEAR(sk, 0.293197777, 1e-9);
}

TEST(QualityMetricsTest, pri6_skewness_1)
{
    std::vector<Point> points = {
        Point(0, 0, 0),    Point(1, 0, 0),    Point(1.5, 1, 0),
        Point(0.25, 0, 2), Point(1.25, 0, 2), Point(1.75, 1, 2),
    };
    std::vector<Element> elements = { Element::Prism6({ 0, 1, 2, 3, 4, 5 }) };
    Mesh mesh(points, elements);

    double sk = qm::skewness<ElementType::PRISM6>(mesh.element(0), mesh);
    EXPECT_NEAR(sk, 0.6698129955, 1e-10);
}

TEST(QualityMetricsTest, pyr5_skewness_1)
{
    std::vector<Point> points = {
        Point(0, 0, 0), Point(1, 0, 0), Point(1.5, 1, 0), Point(0.5, 1, 0), Point(0.75, 0.5, 2),
    };
    std::vector<Element> elements = { Element::Pyramid5({ 0, 1, 2, 3, 4 }) };
    Mesh mesh(points, elements);

    double sk = qm::skewness<ElementType::PYRAMID5>(mesh.element(0), mesh);
    EXPECT_NEAR(sk, 0.3160668323, 1e-10);
}

TEST(QualityMetricsTest, print_stats)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(1, 0, 0), Point(0.5, std::sqrt(3) / 2, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }) };
    Mesh mesh(points, elements);

    auto stats = compute_quality(mesh, qm::Metric::ASPECT_RATIO);
    EXPECT_NEAR(stats.min, 1.0, 1e-12);
    EXPECT_NEAR(stats.max, 1.0, 1e-12);
    EXPECT_EQ(stats.histogram[0], 1);

    print_quality(stats);
}

TEST(QualityMetricsTest, print_stats_complex)
{
    std::vector<Point> points = { Point(0, 0, 0),
                                  Point(1, 0, 0),
                                  Point(0.5, std::sqrt(3) / 2, 0),
                                  Point(2, 0, 0),
                                  Point(1.1, 0.1, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }), Element::Tri3({ 1, 3, 4 }) };
    Mesh mesh(points, elements);

    auto stats = compute_quality(mesh, qm::Metric::ASPECT_RATIO);
    EXPECT_NEAR(stats.min, 1.0, 1e-12);
    EXPECT_NEAR(stats.max, 7.071067811865471, 1e-12);
    EXPECT_EQ(stats.histogram[0], 1);
    EXPECT_EQ(stats.histogram[9], 1);

    print_quality(stats);
}

TEST(QualityMetricsTest, unsupported_metric)
{
    std::vector<Point> points = { Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0) };
    std::vector<Element> elements = { Element::Tri3({ 0, 1, 2 }) };
    Mesh mesh(points, elements);

    EXPECT_THROW(compute_quality(mesh, qm::Metric::SKEWNESS), krado::Exception);
}
