// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/quality_measures.h"
#include "krado/fe_values.h"
#include "krado/utils.h"
#include "krado/vector.h"
#include "krado/point.h"
#include "krado/log.h"
#include "krado/exception.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace krado {

namespace qm {

namespace {

template <ElementType ET>
Point
reference_center()
{
    if constexpr (ET == ElementType::TRI3)
        return Point(1. / 3., 1. / 3., 0.);
    else if constexpr (ET == ElementType::QUAD4)
        return Point(0., 0., 0.);
    else if constexpr (ET == ElementType::TETRA4)
        return Point(1. / 4., 1. / 4., 1. / 4.);
    else if constexpr (ET == ElementType::HEX8)
        return Point(0., 0., 0.);
    else if constexpr (ET == ElementType::PRISM6)
        return Point(1. / 3., 1. / 3., 0.);
    else if constexpr (ET == ElementType::PYRAMID5)
        return Point(0., 0., 0.25);
    else
        return Point(0., 0., 0.);
}

std::string
metric_name(Metric metric)
{
    switch (metric) {
    case Metric::ASPECT_RATIO:
        return "Aspect Ratio";
    case Metric::MIN_ANGLE:
        return "Min Angle";
    case Metric::MAX_ANGLE:
        return "Max Angle";
    case Metric::SCALED_JACOBIAN:
        return "Scaled Jacobian";
    case Metric::ETA:
        return "Eta";
    case Metric::GAMMA:
        return "Gamma";
    case Metric::SKEWNESS:
        return "Skewness";
    default:
        return "Unknown";
    }
}

double
compute_metric(const Element & elem, const Mesh & mesh, Metric metric)
{
    ElementType type = elem.type();
    switch (metric) {
    case Metric::ASPECT_RATIO:
        if (type == ElementType::TRI3)
            return aspect_ratio<ElementType::TRI3>(elem, mesh);
        if (type == ElementType::QUAD4)
            return aspect_ratio<ElementType::QUAD4>(elem, mesh);
        if (type == ElementType::TETRA4)
            return aspect_ratio<ElementType::TETRA4>(elem, mesh);
        if (type == ElementType::HEX8)
            return aspect_ratio<ElementType::HEX8>(elem, mesh);
        if (type == ElementType::PRISM6)
            return aspect_ratio<ElementType::PRISM6>(elem, mesh);
        if (type == ElementType::PYRAMID5)
            return aspect_ratio<ElementType::PYRAMID5>(elem, mesh);
        break;
    case Metric::MIN_ANGLE:
        if (type == ElementType::TRI3)
            return min_angle<ElementType::TRI3>(elem, mesh);
        if (type == ElementType::QUAD4)
            return min_angle<ElementType::QUAD4>(elem, mesh);
        if (type == ElementType::TETRA4)
            return min_angle<ElementType::TETRA4>(elem, mesh);
        if (type == ElementType::HEX8)
            return min_angle<ElementType::HEX8>(elem, mesh);
        if (type == ElementType::PRISM6)
            return min_angle<ElementType::PRISM6>(elem, mesh);
        if (type == ElementType::PYRAMID5)
            return min_angle<ElementType::PYRAMID5>(elem, mesh);
        break;
    case Metric::MAX_ANGLE:
        if (type == ElementType::TRI3)
            return max_angle<ElementType::TRI3>(elem, mesh);
        if (type == ElementType::QUAD4)
            return max_angle<ElementType::QUAD4>(elem, mesh);
        if (type == ElementType::TETRA4)
            return max_angle<ElementType::TETRA4>(elem, mesh);
        if (type == ElementType::HEX8)
            return max_angle<ElementType::HEX8>(elem, mesh);
        if (type == ElementType::PRISM6)
            return max_angle<ElementType::PRISM6>(elem, mesh);
        if (type == ElementType::PYRAMID5)
            return max_angle<ElementType::PYRAMID5>(elem, mesh);
        break;
    case Metric::SCALED_JACOBIAN:
        if (type == ElementType::TRI3)
            return scaled_jacobian<ElementType::TRI3>(elem, mesh);
        if (type == ElementType::QUAD4)
            return scaled_jacobian<ElementType::QUAD4>(elem, mesh);
        if (type == ElementType::TETRA4)
            return scaled_jacobian<ElementType::TETRA4>(elem, mesh);
        if (type == ElementType::HEX8)
            return scaled_jacobian<ElementType::HEX8>(elem, mesh);
        if (type == ElementType::PRISM6)
            return scaled_jacobian<ElementType::PRISM6>(elem, mesh);
        if (type == ElementType::PYRAMID5)
            return scaled_jacobian<ElementType::PYRAMID5>(elem, mesh);
        break;
    case Metric::ETA:
        if (type == ElementType::TRI3)
            return eta<ElementType::TRI3>(elem, mesh);
        if (type == ElementType::QUAD4)
            return eta<ElementType::QUAD4>(elem, mesh);
        if (type == ElementType::TETRA4)
            return eta<ElementType::TETRA4>(elem, mesh);
        if (type == ElementType::HEX8)
            return eta<ElementType::HEX8>(elem, mesh);
        if (type == ElementType::PRISM6)
            return eta<ElementType::PRISM6>(elem, mesh);
        if (type == ElementType::PYRAMID5)
            return eta<ElementType::PYRAMID5>(elem, mesh);
        break;
    case Metric::GAMMA:
        if (type == ElementType::TRI3)
            return gamma<ElementType::TRI3>(elem, mesh);
        if (type == ElementType::TETRA4)
            return gamma<ElementType::TETRA4>(elem, mesh);
        break;
    case Metric::SKEWNESS:
        if (type == ElementType::QUAD4)
            return skewness<ElementType::QUAD4>(elem, mesh);
        if (type == ElementType::HEX8)
            return skewness<ElementType::HEX8>(elem, mesh);
        if (type == ElementType::PRISM6)
            return skewness<ElementType::PRISM6>(elem, mesh);
        if (type == ElementType::PYRAMID5)
            return skewness<ElementType::PYRAMID5>(elem, mesh);
        break;
    default:
        break;
    }
    throw Exception("Metric {} is not supported for element type {}",
                    metric_name(metric),
                    Element::type(type));
}

void
print_histogram(const std::vector<std::size_t> & histogram, double min_val, double max_val)
{
    std::size_t max_count = *std::max_element(histogram.begin(), histogram.end());
    int max_bar_width = 50;

    std::size_t max_count_wd = 0;
    for (auto count : histogram) {
        max_count_wd = std::max(max_count_wd, utils::human_number(count).size());
    }

    int N = histogram.size();
    for (int b = 0; b < N; ++b) {
        double low = min_val + (max_val - min_val) * b / N;
        double high = min_val + (max_val - min_val) * (b + 1) / N;
        std::size_t count = histogram[b];

        int bar_len = (max_count > 0)
                          ? static_cast<int>(static_cast<double>(count) / max_count * max_bar_width)
                          : 0;

        std::string bar(bar_len, '+');
        auto count_str = utils::human_number(count);
        Log::info("  {:.5f} – {:.5f} | {:>{}} {}", low, high, count_str, max_count_wd, bar);
    }
}

} // namespace

template <ElementType ET>
double
aspect_ratio(const Element & elem, const Mesh & mesh)
{
    auto idxs = elem.indices();
    double l_max = 0;
    double l_min = std::numeric_limits<double>::max();
    for (auto & ev : ElementSelector<ET>::edge_vertices()) {
        const auto & p1 = mesh.point(idxs[ev[0]]);
        const auto & p2 = mesh.point(idxs[ev[1]]);
        double l = (p2 - p1).magnitude();
        l_max = std::max(l_max, l);
        l_min = std::min(l_min, l);
    }
    if (l_min == 0)
        return std::numeric_limits<double>::max();
    return l_max / l_min;
}

template <ElementType ET>
std::vector<Vector>
compute_face_normals(const Element & elem, const Mesh & mesh)
{
    auto idxs = elem.indices();
    int n_vtx = ElementSelector<ET>::N_VERTICES;
    Point center(0, 0, 0);
    for (int i = 0; i < n_vtx; ++i)
        center += Vector(mesh.point(idxs[i]));
    center *= (1.0 / n_vtx);

    std::vector<Vector> normals;
    for (auto & fv : ElementSelector<ET>::face_vertices()) {
        const auto & p0 = mesh.point(idxs[fv[0]]);
        const auto & p1 = mesh.point(idxs[fv[1]]);
        const auto & p2 = mesh.point(idxs[fv[2]]);
        Vector n = cross_product(p1 - p0, p2 - p0).normalized();
        if (dot_product(Vector(p0) - Vector(center), n) < 0)
            n = -n;
        normals.push_back(n);
    }
    return normals;
}

template <ElementType ET>
std::vector<double>
compute_angles_deg(const Element & elem, const Mesh & mesh)
{
    auto idxs = elem.indices();
    std::vector<double> angles;
    if constexpr (ET == ElementType::TRI3) {
        angles.push_back(
            utils::angle(mesh.point(idxs[2]), mesh.point(idxs[0]), mesh.point(idxs[1])) * 180. /
            M_PI);
        angles.push_back(
            utils::angle(mesh.point(idxs[0]), mesh.point(idxs[1]), mesh.point(idxs[2])) * 180. /
            M_PI);
        angles.push_back(
            utils::angle(mesh.point(idxs[1]), mesh.point(idxs[2]), mesh.point(idxs[0])) * 180. /
            M_PI);
    }
    else if constexpr (ET == ElementType::QUAD4) {
        angles.push_back(
            utils::angle(mesh.point(idxs[3]), mesh.point(idxs[0]), mesh.point(idxs[1])) * 180. /
            M_PI);
        angles.push_back(
            utils::angle(mesh.point(idxs[0]), mesh.point(idxs[1]), mesh.point(idxs[2])) * 180. /
            M_PI);
        angles.push_back(
            utils::angle(mesh.point(idxs[1]), mesh.point(idxs[2]), mesh.point(idxs[3])) * 180. /
            M_PI);
        angles.push_back(
            utils::angle(mesh.point(idxs[2]), mesh.point(idxs[3]), mesh.point(idxs[0])) * 180. /
            M_PI);
    }
    else {
        auto normals = compute_face_normals<ET>(elem, mesh);
        const auto & face_vtx = ElementSelector<ET>::face_vertices();
        for (std::size_t i = 0; i < face_vtx.size(); ++i) {
            for (std::size_t j = i + 1; j < face_vtx.size(); ++j) {
                int shared = 0;
                for (auto v1 : face_vtx[i])
                    for (auto v2 : face_vtx[j])
                        if (v1 == v2)
                            shared++;
                if (shared >= 2) {
                    double cos_phi = -dot_product(normals[i], normals[j]);
                    cos_phi = std::clamp(cos_phi, -1.0, 1.0);
                    angles.push_back(std::acos(cos_phi) * 180. / M_PI);
                }
            }
        }
    }
    return angles;
}

template <ElementType ET>
double
min_angle(const Element & elem, const Mesh & mesh)
{
    auto angles = compute_angles_deg<ET>(elem, mesh);
    if (angles.empty())
        return 0;
    return *std::min_element(angles.begin(), angles.end());
}

template <ElementType ET>
double
max_angle(const Element & elem, const Mesh & mesh)
{
    auto angles = compute_angles_deg<ET>(elem, mesh);
    if (angles.empty())
        return 0;
    return *std::max_element(angles.begin(), angles.end());
}

template <ElementType ET>
double
scaled_jacobian(const Element & elem, const Mesh & mesh)
{
    Point p = reference_center<ET>();
    auto idxs = elem.indices();
    if constexpr (ET == ElementType::TRI3 || ET == ElementType::QUAD4) {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        for (u8 i = 0; i < ElementSelector<ET>::N_VERTICES; ++i) {
            const auto der = FEValues<ET>::shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
        }
        double det = cross_product(dxdxi, dxdeta).magnitude();
        double scale = dxdxi.magnitude() * dxdeta.magnitude();
        if (scale == 0)
            return 0;
        return det / scale;
    }
    else {
        Vector dxdxi(0, 0, 0);
        Vector dxdeta(0, 0, 0);
        Vector dxdzeta(0, 0, 0);
        for (u8 i = 0; i < ElementSelector<ET>::N_VERTICES; ++i) {
            const auto der = FEValues<ET>::shape_der(i, p);
            const Vector pt(mesh.point(idxs[i]));
            dxdxi += der.x * pt;
            dxdeta += der.y * pt;
            dxdzeta += der.z * pt;
        }
        double det = dot_product(dxdxi, cross_product(dxdeta, dxdzeta));
        double scale = dxdxi.magnitude() * dxdeta.magnitude() * dxdzeta.magnitude();
        if (scale == 0)
            return 0;
        return det / scale;
    }
}

template <ElementType ET>
double
eta(const Element & elem, const Mesh & mesh)
{
    if constexpr (ET == ElementType::TRI3) {
        auto idxs = elem.indices();
        return Tri3::eta(mesh.point(idxs[0]), mesh.point(idxs[1]), mesh.point(idxs[2]));
    }
    else {
        auto angles = compute_angles_deg<ET>(elem, mesh);
        if (angles.empty())
            return 0;
        double theta_e = 60.0;
        if constexpr (ET == ElementType::QUAD4 || ET == ElementType::HEX8)
            theta_e = 90.0;
        else if constexpr (ET == ElementType::TETRA4)
            theta_e = 70.5288; // regular tetrahedron dihedral angle
        else if constexpr (ET == ElementType::PRISM6)
            theta_e = 90.0; // FIXME: prism has both 60 and 90
        else if constexpr (ET == ElementType::PYRAMID5)
            theta_e = 90.0; // FIXME

        double a_min = *std::min_element(angles.begin(), angles.end());
        double a_max = *std::max_element(angles.begin(), angles.end());
        double s_e = std::max((a_max - theta_e) / (180.0 - theta_e), (theta_e - a_min) / theta_e);
        return 1.0 - std::clamp(s_e, 0.0, 1.0);
    }
}

template <ElementType ET>
    requires IsSimplex<ElementSelector<ET>>
double
gamma(const Element & elem, const Mesh & mesh)
{
    auto idxs = elem.indices();
    if constexpr (ET == ElementType::TRI3) {
        return Tri3::gamma(mesh.point(idxs[0]), mesh.point(idxs[1]), mesh.point(idxs[2]));
    }
    else {
        // Tetra4
        const auto & p0 = mesh.point(idxs[0]);
        const auto & p1 = mesh.point(idxs[1]);
        const auto & p2 = mesh.point(idxs[2]);
        const auto & p3 = mesh.point(idxs[3]);
        Vector a = p1 - p0;
        Vector b = p2 - p0;
        Vector c = p3 - p0;
        double vol = std::abs(dot_product(a, cross_product(b, c))) / 6.0;
        if (vol == 0)
            return 0;
        double s1 = cross_product(p1 - p0, p2 - p0).magnitude() / 2.0;
        double s2 = cross_product(p1 - p0, p3 - p0).magnitude() / 2.0;
        double s3 = cross_product(p2 - p1, p3 - p1).magnitude() / 2.0;
        double s4 = cross_product(p2 - p0, p3 - p0).magnitude() / 2.0;
        double r_in = 3.0 * vol / (s1 + s2 + s3 + s4);
        Vector r_out_vec = (a.magnitude() * a.magnitude() * cross_product(b, c) +
                            b.magnitude() * b.magnitude() * cross_product(c, a) +
                            c.magnitude() * c.magnitude() * cross_product(a, b)) *
                           (1.0 / (12.0 * vol));
        double r_out = r_out_vec.magnitude();
        return 3.0 * r_in / r_out;
    }
}

template <ElementType ET>
    requires IsNotSimplex<ElementSelector<ET>>
double
skewness(const Element & elem, const Mesh & mesh)
{
    // For non-simplex elements, we'll use the angles deviation as a measure of skewness
    auto angles = compute_angles_deg<ET>(elem, mesh);
    if (angles.empty())
        return 0;
    double theta_e = 90.0;
    double a_min = *std::min_element(angles.begin(), angles.end());
    double a_max = *std::max_element(angles.begin(), angles.end());
    double s_e = std::max((a_max - theta_e) / (180.0 - theta_e), (theta_e - a_min) / theta_e);
    return std::clamp(s_e, 0.0, 1.0);
}

// Explicit instantiations
template double aspect_ratio<ElementType::TRI3>(const Element &, const Mesh &);
template double aspect_ratio<ElementType::QUAD4>(const Element &, const Mesh &);
template double aspect_ratio<ElementType::TETRA4>(const Element &, const Mesh &);
template double aspect_ratio<ElementType::HEX8>(const Element &, const Mesh &);
template double aspect_ratio<ElementType::PRISM6>(const Element &, const Mesh &);
template double aspect_ratio<ElementType::PYRAMID5>(const Element &, const Mesh &);

template double min_angle<ElementType::TRI3>(const Element &, const Mesh &);
template double min_angle<ElementType::QUAD4>(const Element &, const Mesh &);
template double min_angle<ElementType::TETRA4>(const Element &, const Mesh &);
template double min_angle<ElementType::HEX8>(const Element &, const Mesh &);
template double min_angle<ElementType::PRISM6>(const Element &, const Mesh &);
template double min_angle<ElementType::PYRAMID5>(const Element &, const Mesh &);

template double max_angle<ElementType::TRI3>(const Element &, const Mesh &);
template double max_angle<ElementType::QUAD4>(const Element &, const Mesh &);
template double max_angle<ElementType::TETRA4>(const Element &, const Mesh &);
template double max_angle<ElementType::HEX8>(const Element &, const Mesh &);
template double max_angle<ElementType::PRISM6>(const Element &, const Mesh &);
template double max_angle<ElementType::PYRAMID5>(const Element &, const Mesh &);

template double scaled_jacobian<ElementType::TRI3>(const Element &, const Mesh &);
template double scaled_jacobian<ElementType::QUAD4>(const Element &, const Mesh &);
template double scaled_jacobian<ElementType::TETRA4>(const Element &, const Mesh &);
template double scaled_jacobian<ElementType::HEX8>(const Element &, const Mesh &);
template double scaled_jacobian<ElementType::PRISM6>(const Element &, const Mesh &);
template double scaled_jacobian<ElementType::PYRAMID5>(const Element &, const Mesh &);

template double eta<ElementType::TRI3>(const Element &, const Mesh &);
template double eta<ElementType::QUAD4>(const Element &, const Mesh &);
template double eta<ElementType::TETRA4>(const Element &, const Mesh &);
template double eta<ElementType::HEX8>(const Element &, const Mesh &);
template double eta<ElementType::PRISM6>(const Element &, const Mesh &);
template double eta<ElementType::PYRAMID5>(const Element &, const Mesh &);

template double gamma<ElementType::TRI3>(const Element &, const Mesh &);
template double gamma<ElementType::TETRA4>(const Element &, const Mesh &);

template double skewness<ElementType::QUAD4>(const Element &, const Mesh &);
template double skewness<ElementType::HEX8>(const Element &, const Mesh &);
template double skewness<ElementType::PRISM6>(const Element &, const Mesh &);
template double skewness<ElementType::PYRAMID5>(const Element &, const Mesh &);

} // namespace qm

void
print_quality(const qm::QualityStats & stats)
{
    Log::info("Quality statistics for {}:", qm::metric_name(stats.metric));
    Log::info("  Min: {:.5f}", stats.min);
    Log::info("  Max: {:.5f}", stats.max);

    double range = stats.max - stats.min;
    if (range > 0) {
        qm::print_histogram(stats.histogram, stats.min, stats.max);
    }
    else {
        Log::info("  All elements have the same quality: {:.5f}", stats.min);
    }
}

qm::QualityStats
compute_quality(const Mesh & mesh, qm::Metric metric, int n_bins)
{
    double q_min = std::numeric_limits<double>::max();
    double q_max = -std::numeric_limits<double>::max();
    std::vector<double> qualities;
    qualities.reserve(mesh.num_elements());

    for (const auto & elem : mesh.elements()) {
        double q = qm::compute_metric(elem, mesh, metric);
        q_min = std::min(q_min, q);
        q_max = std::max(q_max, q);
        qualities.push_back(q);
    }

    std::vector<std::size_t> histogram(n_bins, 0);
    double range = q_max - q_min;
    if (range > 0) {
        for (double q : qualities) {
            int bin = static_cast<int>((q - q_min) / range * n_bins);
            if (bin >= n_bins)
                bin = n_bins - 1;
            histogram[bin]++;
        }
    }
    else {
        if (mesh.num_elements() > 0)
            histogram[0] = mesh.num_elements();
    }

    return { metric, q_min, q_max, histogram };
}

} // namespace krado
