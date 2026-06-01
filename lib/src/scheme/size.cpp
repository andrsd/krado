// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/size.h"
#include "krado/scheme/integral.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/vector.h"
#include "krado/utils.h"

namespace krado {

SchemeSize::SchemeSize(Options options) : Scheme("size"), Scheme1D(), opts_(options) {}

std::string
SchemeSize::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("size={}", this->opts_.size));
    return join(", ", spars);
}

void
SchemeSize::mesh_curve(Ptr<MeshCurve> curve)
{
    const auto & geom_curve = curve->geom_curve();

    Integral igrl;
    igrl.integrate(geom_curve, [=](double t) {
        auto der = geom_curve.d1(t);
        return der.magnitude();
    });

    std::size_t n_segs = std::round(geom_curve.length() / this->opts_.size);
    const double b = geom_curve.length() / static_cast<double>(n_segs);
    for (std::size_t count = 1, num_pts = 1; num_pts < n_segs;) {
        auto pt1 = igrl.point(count - 1);
        auto pt2 = igrl.point(count);
        const double d = num_pts * b;
        if ((std::abs(pt2.p) >= std::abs(d)) && (std::abs(pt1.p) < std::abs(d))) {
            const auto dt = pt2.t - pt1.t;
            const auto dp = pt2.p - pt1.p;
            const auto t = pt1.t + dt / dp * (d - pt1.p);
            curve->add_vertex(Ptr<MeshCurveVertex>::alloc(geom_curve, t));
            num_pts++;
        }
        else {
            count++;
        }
    }

    build_curve_segments(curve);
}

} // namespace krado
