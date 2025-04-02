// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/equal.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"

namespace krado {

namespace {

class FEqual {
public:
    FEqual() {}

    double
    operator()(const GeomCurve & curve, double u)
    {
        return curve.d1(u).magnitude() / curve.length();
    }
};

} // namespace

SchemeEqual::SchemeEqual() : SchemeTransfinite("equal") {}

Integral1D
SchemeEqual::compute_integral(const MeshCurve & curve)
{
    auto & geom_curve = curve.geom_curve();
    auto [u_lower, u_upper] = geom_curve.param_range();

    Integral1D integral;
    integral.integrate(geom_curve, u_lower, u_upper, FEqual());
    return integral;
}

} // namespace krado
