// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme/sizemap.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_vertex.h"
#include "krado/numerics.h"
#include "krado/utils.h"
#include "krado/consts.h"
#include <array>

namespace krado {

namespace {

class FTransfiniteSizeMap {
public:
    FTransfiniteSizeMap(const MeshCurve & curve) : mcurve_(curve) {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        auto lc = this->mcurve_.mesh_size_at_param(u);
        return curve.d1(u).magnitude() / lc;
    }

private:
    const MeshCurve & mcurve_;
};

} // namespace

SchemeSizeMap::SchemeSizeMap() : SchemeTransfinite("sizemap") {}

Integral1D
SchemeSizeMap::compute_integral(const MeshCurve & curve)
{
    auto & geom_curve = curve.geom_curve();
    auto [u_lower, u_upper] = geom_curve.param_range();

    Integral1D integral;
    integral.integrate(geom_curve, u_lower, u_upper, FTransfiniteSizeMap(curve));
    return integral;
}

} // namespace krado
