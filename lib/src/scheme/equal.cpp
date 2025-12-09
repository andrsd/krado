// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/equal.h"
#include "krado/scheme/integral.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/log.h"
#include "krado/ptr.h"

namespace krado {

SchemeEqual::SchemeEqual(Options options) : Scheme1D("equal"), opts_(options) {}

void
SchemeEqual::mesh_curve(Ptr<MeshCurve> curve)
{
    auto & geom_curve = curve->geom_curve();
    auto n_segs = this->opts_.intervals;

    Log::info("Meshing curve {}: scheme='equal', intervals={}", curve->id(), n_segs);

    // compute arc length
    Integral igrl;
    igrl.integrate(geom_curve, [=](double t) {
        auto der = geom_curve.d1(t);
        return der.magnitude();
    });

    const double b = geom_curve.length() / static_cast<double>(n_segs);
    std::vector<Ptr<MeshCurveVertex>> curve_vtxs;
    for (int count = 1, num_pts = 1; num_pts < n_segs;) {
        auto & pt1 = igrl.point(count - 1);
        auto & pt2 = igrl.point(count);
        const auto d = (double) num_pts * b;
        if ((std::abs(pt2.p) >= std::abs(d)) && (std::abs(pt1.p) < std::abs(d))) {
            const auto dt = pt2.t - pt1.t;
            const auto dlc = pt2.lc - pt1.lc;
            const auto dp = pt2.p - pt1.p;
            const auto t = pt1.t + dt / dp * (d - pt1.p);
            curve_vtxs.push_back(Ptr<MeshCurveVertex>::alloc(geom_curve, t));
            num_pts++;
        }
        else {
            count++;
        }
    }

    auto bnd_verts = curve->bounding_vertices();
    if ((geom_curve.type() == GeomCurve::CurveType::Circle) && (bnd_verts.size() == 1)) {
        // curve is a full circle
        curve->add_vertex(bnd_verts[0]);
        for (auto & cv : curve_vtxs)
            curve->add_vertex(cv);

        for (std::size_t i = 0; i < n_segs - 1; ++i)
            curve->add_segment({ curve->all_vertices()[i], curve->all_vertices()[i + 1] });
        curve->add_segment({ curve->all_vertices()[n_segs - 1], bnd_verts[0] });
    }
    else {
        curve->add_vertex(bnd_verts[0]);
        for (auto & cv : curve_vtxs)
            curve->add_vertex(cv);
        curve->add_vertex(bnd_verts[1]);

        for (std::size_t i = 0; i < n_segs; ++i)
            curve->add_segment({ curve->all_vertices()[i], curve->all_vertices()[i + 1] });
    }
}

} // namespace krado
