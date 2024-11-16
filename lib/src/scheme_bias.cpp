// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme_bias.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/numerics.h"
#include "krado/utils.h"
#include <array>

namespace krado {

namespace {

// Geometric progression ar^i; Sum of n terms = length = a (r^n-1)/(r-1)
class FTransfiniteSizeMap {
public:
    FTransfiniteSizeMap(double coef, int n_pts, int ori) : coef(coef), n_points(n_pts), ori(ori)
    {
    }

    double
    operator()(const GeomCurve & curve, double u)
    {
        double length = curve.length();

        double d = curve.d1(u).norm();

        auto [u_lower, u_upper] = curve.param_range();
        double t = (u - u_lower) / (u_upper - u_lower);

        double r = (sign(this->ori) >= 0) ? this->coef : 1. / this->coef;
        double a = length * (r - 1.) / (std::pow(r, this->n_points - 1.) - 1.);
        int i = (int) (std::log(t * length / a * (r - 1.) + 1.) / std::log(r));
        return d / (a * std::pow(r, (double) i));
    }

private:
    /// Coefficient
    double coef;
    /// Number of points
    int n_points;
    /// Orientation of the curve
    int ori;
};

} // namespace

SchemeBias::SchemeBias() : SchemeTransfinite("bias")
{
}

Integral1D
SchemeBias::compute_integral(const MeshCurve & curve)
{
    auto coeff = get<double>("coef");
    auto n_intervals = get<int>("intervals");
    auto n_pts = n_intervals + 1;

    auto & geom_curve = curve.geom_curve();
    auto [u_lower, u_upper] = geom_curve.param_range();

    Integral1D integral;
    integral.integrate(geom_curve, u_lower, u_upper, FTransfiniteSizeMap(coeff, n_pts, 1));
    return integral;
}

} // namespace krado
