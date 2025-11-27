// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/equal.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"
#include "krado/log.h"
#include "krado/ptr.h"
#include "GCPnts_AbscissaPoint.hxx"

namespace krado {
namespace {

std::vector<Ptr<MeshCurveVertex>>
mesh_curve_by_count(const GeomCurve & curve, int n_segments, double tol = 1e-8)
{
    std::vector<Ptr<MeshCurveVertex>> vertices;
    vertices.reserve(n_segments - 1);

    GeomAdaptor_Curve adaptor(curve.curve_handle());
    auto [t0, t1] = curve.param_range();

    Standard_Real length = GCPnts_AbscissaPoint::Length(adaptor, t0, t1, tol);
    Standard_Real step = length / n_segments;
    Standard_Real current_param = t0;
    for (int i = 1; i < n_segments; ++i) {
        GCPnts_AbscissaPoint abscissa(adaptor, step, current_param, tol);
        if (!abscissa.IsDone())
            throw Exception("Failed to compute abscissa point");

        current_param = abscissa.Parameter();
        vertices.push_back(Ptr<MeshCurveVertex>::alloc(curve, current_param));
    }

    return vertices;
}

} // namespace

SchemeEqual::SchemeEqual(Options options) : Scheme1D("equal"), opts_(options) {}

void
SchemeEqual::mesh_curve(Ptr<MeshCurve> curve)
{
    auto & geom_curve = curve->geom_curve();
    auto n_intervals = this->opts_.intervals;

    Log::info("Meshing curve {}: scheme='equal', intervals={}", curve->id(), n_intervals);

    if (geom_curve.type() == GeomCurve::CurveType::Circle) {
        auto bnd_verts = curve->bounding_vertices();
        auto curve_vtxs = mesh_curve_by_count(geom_curve, n_intervals);
        curve->add_vertex(bnd_verts[0]);
        for (auto & cv : curve_vtxs)
            curve->add_vertex(cv);

        for (std::size_t i = 0; i < n_intervals - 1; ++i)
            curve->add_segment({ curve->all_vertices()[i], curve->all_vertices()[i + 1] });
        curve->add_segment({ curve->all_vertices()[n_intervals - 1], bnd_verts[0] });
    }
    else {
        auto bnd_verts = curve->bounding_vertices();
        auto curve_vtxs = mesh_curve_by_count(geom_curve, n_intervals);
        curve->add_vertex(bnd_verts[0]);
        for (auto & cv : curve_vtxs)
            curve->add_vertex(cv);
        curve->add_vertex(bnd_verts[1]);

        for (std::size_t i = 0; i < n_intervals; ++i)
            curve->add_segment({ curve->all_vertices()[i], curve->all_vertices()[i + 1] });
    }
}

} // namespace krado
