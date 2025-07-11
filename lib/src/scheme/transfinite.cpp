// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme/transfinite.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/log.h"
#include "krado/utils.h"
#include <array>

namespace krado {

namespace {

std::vector<MeshCurveVertex *>
build_curve_vertices(const GeomCurve & curve, const Integral1D & integral, int n)
{
    std::vector<MeshCurveVertex *> vertices;

    // do not consider the first and the last vertex (those are not classified on this mesh edge)
    if (n > 1) {
        const double b = integral.value() / static_cast<double>(n - 1);
        int i = 1;
        int n_points = 1;
        vertices.resize(n - 2);

        while (n_points < n - 1) {
            auto p1 = integral.point(i - 1);
            auto p2 = integral.point(i);
            const double d = (double) n_points * b;
            if ((std::abs(p1.p) < std::abs(d)) && (std::abs(d) <= std::abs(p2.p))) {
                double const dt = p2.t - p1.t;
                // double const dlc = p2.lc - p1.lc;
                double const dp = p2.p - p1.p;
                double const t = p1.t + dt / dp * (d - p1.p);
                // const double d1_mag = curve.d1(t).norm();
                // double lc = d1_mag / (p1.lc + dlc / dp * (d1_mag - p1.p));
                vertices[n_points - 1] = new MeshCurveVertex(curve, t);
                // TODO: store Lc in mesh curve vertex
                n_points++;
            }
            else {
                i++;
            }
        }
        vertices.resize(n_points - 1);
    }

    return vertices;
}

} // namespace

SchemeTransfinite::SchemeTransfinite(const std::string & name) : Scheme(name) {}

void
SchemeTransfinite::mesh_curve(MeshCurve & curve)
{
    auto n_intervals = get<int>("intervals");

    Log::info("Meshing curve {}: scheme='{}', intervals={}", curve.id(), name(), n_intervals);

    auto & geom_curve = curve.geom_curve();
    auto integral = compute_integral(curve);

    auto n_pts = n_intervals + 1;
    std::vector<MeshCurveVertex *> curve_vtxs = build_curve_vertices(geom_curve, integral, n_pts);
    auto & bnd_verts = curve.bounding_vertices();
    curve.add_vertex(bnd_verts[0]);
    for (auto & cv : curve_vtxs)
        curve.add_vertex(cv);
    if (bnd_verts.size() == 2)
        curve.add_vertex(bnd_verts[1]);
    for (std::size_t i = 0; i < n_intervals; i++)
        curve.add_segment({ curve.all_vertices()[i], curve.all_vertices()[i + 1] });
}

} // namespace krado
