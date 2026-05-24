// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/curvature.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/log.h"
#include "krado/exception.h"
#include "krado/utils.h"
#include "GCPnts_AbscissaPoint.hxx"
#include "GeomAdaptor_Curve.hxx"

namespace krado {

SchemeCurvature::SchemeCurvature(Options options) : Scheme1D("curvature"), opts_(options) {}

void
SchemeCurvature::on_mesh_curve(Ptr<MeshCurve> curve)
{
    const auto & geom_curve = curve->geom_curve();
    GeomAdaptor_Curve adaptor(geom_curve.curve_handle());

    auto [t0, t1] = geom_curve.param_range();
    const double tol = 1e-6;

    auto total_length = GCPnts_AbscissaPoint::Length(adaptor, t0, t1, tol);
    if (total_length < tol) {
        Log::warn("Curve {} is too small (length={})", curve->id(), total_length);
        curve->set_too_small(true);
        return;
    }

    const auto & bnd_verts = curve->bounding_vertices();
    if (bnd_verts.empty())
        throw Exception("Curve {} must have at least 1 bounding vertex", curve->id());

    double s = 0.0;
    double t = t0;
    while (s < total_length - tol) {
        auto u = t;
        auto kappa = geom_curve.curvature(u);
        double h;
        if (kappa > 1e-12) {
            h = this->opts_.deflection / kappa;
        }
        else {
            h = this->opts_.max_size;
        }

        h = std::max(h, this->opts_.min_size);
        h = std::min(h, this->opts_.max_size);

        if (s + h > total_length)
            h = total_length - s;

        GCPnts_AbscissaPoint abscissa(adaptor, h, t, tol);
        if (!abscissa.IsDone())
            throw Exception("Failed to compute abscissa point");

        t = abscissa.Parameter();
        if (t < t1 - tol) {
            auto cvtx = Ptr<MeshCurveVertex>::alloc(geom_curve, t);
            curve->add_vertex(cvtx);
            s += h;
        }
        else
            // Reached (or passed) end parameter
            break;
    }

    build_curve_segments(curve);
}

} // namespace krado
