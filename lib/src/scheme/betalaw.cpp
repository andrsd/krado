// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This is a rewrite of `meshGEdge` from gmsh

#include "krado/scheme/betalaw.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"

namespace krado {

namespace {

/// Beta law
class FBetaLaw {
public:
    FBetaLaw(double coef, int ori) : coef(coef), ori(ori) {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        auto [u_lower, u_hi] = curve.param_range();
        double t = (u - u_lower) / (u_hi - u_lower);

        if (this->coef < 1.) {
            auto d = curve.d1(u).magnitude();
            return d * this->coef / curve.length();
        }
        else {
            // "beta" law
            if (this->ori < 0)
                return dfbeta(1. - t, this->coef);
            else
                return dfbeta(t, this->coef);
        }
    }

private:
    /// Coefficient
    double coef;
    /// Orientation
    int ori;

    double
    dfbeta(double t, double beta)
    {
        double ratio = (1 + beta) / (beta - 1);
        double zlog = std::log(ratio);
        return 2 * beta / ((1 + beta - t) * (-1 + beta + t) * zlog);
    }
};

} // namespace

SchemeBetaLaw::SchemeBetaLaw() : SchemeTransfinite("bias") {}

Integral1D
SchemeBetaLaw::compute_integral(const MeshCurve & curve)
{
    Integral1D integral;
    auto & geom_curve = curve.geom_curve();
    auto [u_lower, u_upper] = geom_curve.param_range();
    auto coeff = get<double>("coef");
    integral.integrate(geom_curve, u_lower, u_upper, FBetaLaw(coeff, 1));
    return integral;
}

} // namespace krado
