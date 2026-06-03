// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/curvature.h"
#include "krado/integral.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/log.h"
#include "krado/exception.h"
#include "krado/utils.h"
#include "krado/vector.h"
#include <cmath>
#include <algorithm>

namespace krado {

SchemeCurvature::SchemeCurvature(Options options) : Scheme("curvature"), Scheme1D(), opts_(options)
{
}

std::string
SchemeCurvature::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("min_size={}", this->opts_.min_size));
    spars.push_back(fmt::format("max_size={}", this->opts_.max_size));
    spars.push_back(fmt::format("deflection={}", this->opts_.deflection));
    return join(", ", spars);
}

void
SchemeCurvature::mesh_curve(Ptr<MeshCurve> curve)
{
    const auto & geom_curve = curve->geom_curve();

    Integral igrl;
    igrl.integrate(geom_curve, [&](double t) {
        auto der = geom_curve.d1(t);
        auto ds = der.magnitude();
        auto kappa = geom_curve.curvature(t);
        double h;
        if (kappa > 1e-12)
            h = this->opts_.deflection / kappa;
        else
            h = this->opts_.max_size;

        h = std::max(h, this->opts_.min_size);
        h = std::min(h, this->opts_.max_size);

        return ds / h;
    });

    auto total_weight = igrl.point(igrl.num_points() - 1).p;
    if (total_weight < 1e-6) {
        Log::warn("Curve {} is too small for curvature scheme", curve->id());
        curve->set_too_small(true);
        return;
    }

    std::size_t n_segs = std::max(1, static_cast<int>(std::round(total_weight)));
    auto delta_weight = total_weight / static_cast<double>(n_segs);

    for (std::size_t count = 1, num_pts = 1; num_pts < n_segs;) {
        auto pt1 = igrl.point(count - 1);
        auto pt2 = igrl.point(count);
        const auto d = static_cast<double>(num_pts) * delta_weight;
        if ((std::abs(pt2.p) >= std::abs(d)) && (std::abs(pt1.p) < std::abs(d))) {
            const auto dt = pt2.t - pt1.t;
            const auto dp = pt2.p - pt1.p;
            const auto t = pt1.t + dt / dp * (d - pt1.p);
            curve->add_vertex(Ptr<MeshCurveVertex>::alloc(geom_curve, t));
            num_pts++;
        }
        else {
            count++;
            if (count >= igrl.num_points())
                break;
        }
    }

    build_curve_segments(curve);
}

} // namespace krado
