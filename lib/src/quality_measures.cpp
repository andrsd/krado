// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/quality_measures.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/vector.h"
#include "krado/utils.h"
#include "krado/types.h"
#include "krado/robust_predicates.h"
#include <array>

namespace krado {
namespace quality {

namespace {

/// Compute eta quality measure for a given element
template <ElementType ET>
double qm_eta(const MeshElement & e);

template <>
double
qm_eta<ElementType::TRI3>(const MeshElement & el)
{
    std::array<Point, 3> pt = { el.vertex(0)->point(),
                                el.vertex(1)->point(),
                                el.vertex(2)->point() };

    auto a1 = 180 * utils::angle(pt[0], pt[1], pt[2]) / M_PI;
    auto a2 = 180 * utils::angle(pt[1], pt[2], pt[0]) / M_PI;
    auto a3 = 180 * utils::angle(pt[2], pt[0], pt[1]) / M_PI;

    auto amin = std::min(std::min(a1, a2), a3);
    auto angle = std::abs(60. - amin);
    return 1. - angle / 60.;
}

template <>
double
qm_eta<ElementType::QUAD4>(const MeshElement & el)
{
    double AR = 1; // pow(el->minEdge()/el->maxEdge(),.25);

    std::array<Point, 4> pt { el.vertex(0)->point(),
                              el.vertex(1)->point(),
                              el.vertex(2)->point(),
                              el.vertex(3)->point() };

    auto v01 = pt[1] - pt[0];
    auto v12 = pt[2] - pt[1];
    auto v23 = pt[3] - pt[2];
    auto v30 = pt[0] - pt[3];

    auto a = cross_product(v01, v12);
    auto b = cross_product(v12, v23);
    auto c = cross_product(v23, v30);
    auto d = cross_product(v30, v01);

    double sign = 1.0;
    if (dot_product(a, b) < 0 || dot_product(a, c) < 0 || dot_product(a, d) < 0)
        sign = -1;
    // FIXME ...
    //  if (a.z() > 0 || b.z() > 0 || c.z() > 0 || d.z() > 0) sign = -1;

    double a1 = 180 * utils::angle(pt[0], pt[1], pt[2]) / M_PI;
    double a2 = 180 * utils::angle(pt[1], pt[2], pt[3]) / M_PI;
    double a3 = 180 * utils::angle(pt[2], pt[3], pt[0]) / M_PI;
    double a4 = 180 * utils::angle(pt[3], pt[0], pt[1]) / M_PI;

    a1 = std::min(180., a1);
    a2 = std::min(180., a2);
    a3 = std::min(180., a3);
    a4 = std::min(180., a4);
    auto angle = std::abs(90. - a1);
    angle = std::max(fabs(90. - a2), angle);
    angle = std::max(fabs(90. - a3), angle);
    angle = std::max(fabs(90. - a4), angle);

    return sign * (1. - angle / 90) * AR;
}

template <>
double
qm_eta<ElementType::TETRA4>(const MeshElement & el)
{
    std::array<Point, 4> pt { el.vertex(0)->point(),
                              el.vertex(1)->point(),
                              el.vertex(2)->point(),
                              el.vertex(3)->point() };

    double p0[3] = { pt[0].x, pt[0].y, pt[0].z };
    double p1[3] = { pt[1].x, pt[1].y, pt[1].z };
    double p2[3] = { pt[2].x, pt[2].y, pt[2].z };
    double p3[3] = { pt[3].x, pt[3].y, pt[3].z };
    auto volume = std::abs(robust_predicates::orient3d(p0, p1, p2, p3)) / 6.0;

    auto & x1 = pt[0].x;
    auto & y1 = pt[0].y;
    auto & z1 = pt[0].z;
    auto & x2 = pt[1].x;
    auto & y2 = pt[1].y;
    auto & z2 = pt[1].z;
    auto & x3 = pt[2].x;
    auto & y3 = pt[2].y;
    auto & z3 = pt[2].z;
    auto & x4 = pt[3].x;
    auto & y4 = pt[3].y;
    auto & z4 = pt[3].z;

    auto l = ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
    l += ((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) + (z3 - z1) * (z3 - z1));
    l += ((x4 - x1) * (x4 - x1) + (y4 - y1) * (y4 - y1) + (z4 - z1) * (z4 - z1));
    l += ((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) + (z3 - z2) * (z3 - z2));
    l += ((x4 - x2) * (x4 - x2) + (y4 - y2) * (y4 - y2) + (z4 - z2) * (z4 - z2));
    l += ((x3 - x4) * (x3 - x4) + (y3 - y4) * (y3 - y4) + (z3 - z4) * (z3 - z4));
    return 12. * std::pow(3 * std::abs(volume), 2. / 3.) / l;
}

} // namespace

double
eta(const MeshElement & e)
{
    if (e.type() == ElementType::TRI3)
        return qm_eta<ElementType::TRI3>(e);
    else if (e.type() == ElementType::QUAD4)
        return qm_eta<ElementType::QUAD4>(e);
    else if (e.type() == ElementType::TETRA4)
        return qm_eta<ElementType::TETRA4>(e);
    else
        return 0.;
}

} // namespace quality
} // namespace krado
