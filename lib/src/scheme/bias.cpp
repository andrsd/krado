// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/bias.h"
#include "krado/scheme/integral.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/vector.h"
#include "krado/utils.h"
#include "fmt/core.h"

namespace krado {

SchemeBias::SchemeBias(Options options) : Scheme("bias"), Scheme1D(), opts_(options) {}

std::string
SchemeBias::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("intervals={}", this->opts_.intervals));
    spars.push_back(fmt::format("factor={}", this->opts_.factor));
    return join(", ", spars);
}

void
SchemeBias::mesh_curve(Ptr<MeshCurve> curve)
{
    auto & geom_curve = curve->geom_curve();
    std::size_t n_segs = this->opts_.intervals;
    auto bias_factor = this->opts_.factor;

    Integral igrl;
    igrl.integrate(geom_curve, [=](double t) {
        auto der = geom_curve.d1(t);
        return der.magnitude();
    });

    // place mesh curve vertices
    double l0 = geom_curve.length() * (bias_factor - 1.) / (std::pow(bias_factor, n_segs) - 1);
    double p_prev = 0.;
    for (std::size_t count = 1, num_pts = 0; num_pts < n_segs - 1;) {
        auto pt1 = igrl.point(count - 1);
        auto pt2 = igrl.point(count);
        const auto d = p_prev + l0 * std::pow(bias_factor, num_pts);
        if ((std::abs(pt2.p) >= std::abs(d)) && (std::abs(pt1.p) < std::abs(d))) {
            p_prev = d;
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
