// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/size.h"
#include "krado/scheme/equal.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/log.h"
#include "GCPnts_AbscissaPoint.hxx"

namespace krado {

SchemeSize::SchemeSize(Options options) : Scheme1D("size"), opts_(options) {}

void
SchemeSize::mesh_curve(Ptr<MeshCurve> curve)
{
    Log::info("Meshing curve {}: scheme='size'", curve->id());

    const auto & geom_curve = curve->geom_curve();
    GeomAdaptor_Curve adaptor(geom_curve.curve_handle());

    auto [t0, t1] = geom_curve.param_range();
    const double tol = 1e-6;

    double total_length = GCPnts_AbscissaPoint::Length(adaptor, t0, t1, tol);
    if (total_length < tol) {
        Log::warn("Curve {} is too small (length={})", curve->id(), total_length);
        curve->set_too_small(true);
        return;
    }

    const auto & bnd_verts = curve->bounding_vertices();
    if (bnd_verts.size() != 2)
        throw Exception("Curve {} must have 2 bounding vertices", curve->id());

    curve->add_vertex(bnd_verts[0]);

    double s = 0.0;
    double t = t0;
    while (s < total_length - tol) {
        auto u = t;
        auto h = curve->mesh_size_at_param(u);

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

    curve->add_vertex(bnd_verts[1]);

    const auto & all = curve->all_vertices();
    for (std::size_t i = 0; i + 1 < all.size(); ++i) {
        curve->add_segment({ all[i], all[i + 1] });
    }
}

} // namespace krado
