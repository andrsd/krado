// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme/sizemap.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/numerics.h"
#include "krado/utils.h"
#include <array>

namespace krado {

namespace {

class FTransfiniteSizeMap {
public:
    FTransfiniteSizeMap() {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        // auto p = curve.point(u);
        // auto [u_lo, u_hi] = curve.param_range();
        double lc_here = 1.e22;

        // if(t == t_begin && ge->getBeginVertex())
        //     lc_here = BGM_MeshSize(ge->getBeginVertex(), t, 0, p.x(), p.y(), p.z());
        // else if(t == t_end && ge->getEndVertex())
        //     lc_here = BGM_MeshSize(ge->getEndVertex(), t, 0, p.x(), p.y(), p.z());
        // lc_here = std::min(lc_here, BGM_MeshSize(ge, t, 0, p.x(), p.y(), p.z()));
        return curve.d1(u).magnitude() / lc_here;
    }

private:
};

} // namespace

SchemeSizeMap::SchemeSizeMap() : SchemeTransfinite("sizemap") {}

Integral1D
SchemeSizeMap::compute_integral(const MeshCurve & curve)
{
    // auto coeff = get<double>("coef");
    // auto n_intervals = get<int>("intervals");
    // auto n_pts = n_intervals + 1;

    auto & geom_curve = curve.geom_curve();
    auto [u_lower, u_upper] = geom_curve.param_range();

    Integral1D integral;
    integral.integrate(geom_curve, u_lower, u_upper, FTransfiniteSizeMap());
    return integral;
}

} // namespace krado
