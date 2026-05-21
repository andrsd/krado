// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/equal.h"
#include "krado/scheme/integral.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/vector.h"
#include "krado/log.h"
#include "krado/ptr.h"
#include "krado/utils.h"

namespace krado {

SchemeEqual::SchemeEqual(Options options) : Scheme1D("equal"), opts_(options) {}

void
SchemeEqual::mesh_curve(Ptr<MeshCurve> curve)
{
    auto & geom_curve = curve->geom_curve();
    auto n_segs = this->opts_.intervals;

    Log::info("Meshing curve {}: scheme='equal', intervals={}", curve->id(), n_segs);

    Integral igrl;
    igrl.integrate(geom_curve, [=](double t) {
        auto der = geom_curve.d1(t);
        return der.magnitude();
    });

    const double b = geom_curve.length() / static_cast<double>(n_segs);
    for (int count = 1, num_pts = 1; num_pts < n_segs;) {
        auto pt1 = igrl.point(count - 1);
        auto pt2 = igrl.point(count);
        const auto d = (double) num_pts * b;
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
