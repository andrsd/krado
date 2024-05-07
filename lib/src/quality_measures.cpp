// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/quality_measures.h"
#include "krado/element.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/utils.h"
#include "krado/types.h"
#include "krado/predicates.h"
#include "krado/bds.h"
#include <array>

namespace krado {
namespace quality {

namespace {

/// Compute eta quality measure for a given element
template <ElementType ET>
double qm_eta(const std::array<Point, ElementSelector<ET>::N_VERTICES> & pts);

template <>
double
qm_eta<ElementType::TRI3>(const std::array<Point, 3> & pt)
{
    auto a1 = 180 * utils::angle(pt[0], pt[1], pt[2]) / M_PI;
    auto a2 = 180 * utils::angle(pt[1], pt[2], pt[0]) / M_PI;
    auto a3 = 180 * utils::angle(pt[2], pt[0], pt[1]) / M_PI;

    auto amin = std::min(std::min(a1, a2), a3);
    auto angle = std::abs(60. - amin);
    return 1. - angle / 60.;
}

template <>
double
qm_eta<ElementType::QUAD4>(const std::array<Point, 4> & pt)
{
    double AR = 1; // pow(el->minEdge()/el->maxEdge(),.25);

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
qm_eta<ElementType::TETRA4>(const std::array<Point, 4> & pt)
{
    auto volume = std::abs(orient3d(pt[0], pt[1], pt[2], pt[3])) / 6.0;

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

/// gamma quality measure for a given element
template <ElementType ET>
double qm_gamma(const std::array<Point, ElementSelector<ET>::N_VERTICES> & pts);

template <>
double
qm_gamma<ElementType::TRI3>(const std::array<Point, 3> & pt)
{
    // quality = rho / R = 2 * inscribed radius / circumradius
    auto a = pt[2] - pt[1];
    auto b = pt[0] - pt[2];
    auto c = pt[1] - pt[0];
    a.normalize();
    b.normalize();
    c.normalize();
    auto pva = cross_product(b, c);
    const auto sina = pva.magnitude();
    auto pvb = cross_product(c, a);
    const auto sinb = pvb.magnitude();
    auto pvc = cross_product(a, b);
    const auto sinc = pvc.magnitude();
    if (sina == 0.0 && sinb == 0.0 && sinc == 0.0)
        return 0.0;
    else
        return 2 * (2 * sina * sinb * sinc / (sina + sinb + sinc));
}

template <>
double
qm_gamma<ElementType::QUAD4>(const std::array<Point, 4> & pt)
{
    return qm_eta<ElementType::QUAD4>(pt);
}

template <>
double
qm_gamma<ElementType::TETRA4>(const std::array<Point, 4> & pt)
{
    // quality = rho / R = 3 * inradius / circumradius

    auto volume = orient3d(pt[0], pt[1], pt[2], pt[3]) / 6.0;

    if (std::fabs(volume) == 0)
        return 0;

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

    auto la = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
    auto lb = (x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1) + (z3 - z1) * (z3 - z1);
    auto lc = (x4 - x1) * (x4 - x1) + (y4 - y1) * (y4 - y1) + (z4 - z1) * (z4 - z1);
    auto lA = (x4 - x3) * (x4 - x3) + (y4 - y3) * (y4 - y3) + (z4 - z3) * (z4 - z3);
    auto lB = (x4 - x2) * (x4 - x2) + (y4 - y2) * (y4 - y2) + (z4 - z2) * (z4 - z2);
    auto lC = (x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) + (z3 - z2) * (z3 - z2);

    auto lalA = std::sqrt(la * lA);
    auto lblB = std::sqrt(lb * lB);
    auto lclC = std::sqrt(lc * lC);

    auto inside_sqrt =
        (lalA + lblB + lclC) * (lalA + lblB - lclC) * (lalA - lblB + lclC) * (-lalA + lblB + lclC);

    // This happens when the 4 points are (nearly) co-planar
    // => R is actually undetermined but the quality is (close to) zero
    if (inside_sqrt <= 0)
        return 0;

    auto R = std::sqrt(inside_sqrt) / 24. / volume;

    auto s1 = std::fabs(utils::triangle_area(pt[0], pt[1], pt[2]));
    auto s2 = std::fabs(utils::triangle_area(pt[0], pt[2], pt[3]));
    auto s3 = std::fabs(utils::triangle_area(pt[0], pt[1], pt[3]));
    auto s4 = std::fabs(utils::triangle_area(pt[1], pt[2], pt[3]));
    auto rho = 3. * 3. * volume / (s1 + s2 + s3 + s4);

    return rho / R;
}

} // namespace

double
eta(const MeshElement & e)
{
    if (e.type() == ElementType::TRI3) {
        std::array<Point, 3> pts = { e.vertex(0)->point(),
                                     e.vertex(1)->point(),
                                     e.vertex(2)->point() };
        return qm_eta<ElementType::TRI3>(pts);
    }
    else if (e.type() == ElementType::QUAD4) {
        std::array<Point, 4> pts { e.vertex(0)->point(),
                                   e.vertex(1)->point(),
                                   e.vertex(2)->point(),
                                   e.vertex(3)->point() };
        return qm_eta<ElementType::QUAD4>(pts);
    }
    else if (e.type() == ElementType::TETRA4) {
        std::array<Point, 4> pts { e.vertex(0)->point(),
                                   e.vertex(1)->point(),
                                   e.vertex(2)->point(),
                                   e.vertex(3)->point() };
        return qm_eta<ElementType::TETRA4>(pts);
    }
    else
        return 0.;
}

double
gamma(const MeshElement & e)
{
    if (e.type() == ElementType::TRI3) {
        std::array<Point, 3> pts = { e.vertex(0)->point(),
                                     e.vertex(1)->point(),
                                     e.vertex(2)->point() };
        return qm_gamma<ElementType::TRI3>(pts);
    }
    else if (e.type() == ElementType::QUAD4) {
        std::array<Point, 4> pts { e.vertex(0)->point(),
                                   e.vertex(1)->point(),
                                   e.vertex(2)->point(),
                                   e.vertex(3)->point() };
        return qm_gamma<ElementType::QUAD4>(pts);
    }
    else if (e.type() == ElementType::TETRA4) {
        std::array<Point, 4> pts { e.vertex(0)->point(),
                                   e.vertex(1)->point(),
                                   e.vertex(2)->point(),
                                   e.vertex(3)->point() };
        return qm_gamma<ElementType::TETRA4>(pts);
    }
    else
        return 0.;
}

template <>
double
gamma<ElementType::TRI3>(const std::array<BDS_Point *, 3> & bds_pts)
{
    std::array<Point, 3> pts = { Point(bds_pts[0]->x, bds_pts[0]->y, bds_pts[0]->z),
                                 Point(bds_pts[1]->x, bds_pts[1]->y, bds_pts[1]->z),
                                 Point(bds_pts[2]->x, bds_pts[2]->y, bds_pts[2]->z) };
    return qm_gamma<ElementType::TRI3>(pts);
}

} // namespace quality
} // namespace krado
