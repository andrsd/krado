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
