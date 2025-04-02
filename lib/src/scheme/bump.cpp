// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme/bump.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"

namespace krado {

namespace {

// "bump"
class FBump {
public:
    FBump(double coef, int n_pts) : coef(coef), n_points(n_pts) {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        double length = curve.length();

        double d = curve.d1(u).magnitude();
        auto [u_lower, u_upper] = curve.param_range();
        double t = (u - u_lower) / (u_upper - u_lower);

        double a;
        if (this->coef > 1.0) {
            a = -4. * std::sqrt(this->coef - 1.) * std::atan2(1.0, std::sqrt(this->coef - 1.)) /
                ((double) this->n_points * length);
        }
        else {
            a = 2. * std::sqrt(1. - this->coef) *
                std::log(std::abs((1. + 1. / std::sqrt(1. - this->coef)) /
                                  (1. - 1. / std::sqrt(1. - this->coef)))) /
                ((double) this->n_points * length);
        }
        double b = -a * length * length / (4. * (this->coef - 1.));
        return d / (-a * std::pow(t * length - (length) *0.5, 2) + b);
    }

public:
    /// Coefficient
    double coef;
    // Number of points
    int n_points;
};

} // namespace

SchemeBump::SchemeBump() : SchemeTransfinite("bump") {}

Integral1D
SchemeBump::compute_integral(const MeshCurve & curve)
{
    Integral1D integral;
    auto & geom_curve = curve.geom_curve();
    auto [u_lower, u_upper] = geom_curve.param_range();
    auto coeff = get<double>("coef");
    auto n_intervals = get<int>("intervals");
    auto n_pts = n_intervals + 1;
    integral.integrate(geom_curve, u_lower, u_upper, FBump(coeff, n_pts));
    return integral;
}

} // namespace krado
