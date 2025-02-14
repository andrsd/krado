// SPDX-FileCopyrightText: Copyright (C) 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "krado/geom_curve.h"
#include <vector>

namespace krado {

class Integral1D {
public:
    struct IntPoint {
        int num;
        // The local coordinate of the point
        double t;
        // lc is x'(t)/h(x(t))
        double lc;
        // The value of the primitive
        double p;
        // The norm of the tangent vector
        double xp;
    };

    [[nodiscard]] double
    value() const
    {
        return this->val;
    }

    [[nodiscard]] const IntPoint &
    point(int idx) const
    {
        return this->pts[idx];
    }

    template <typename FUNCTION>
    void
    integrate(const GeomCurve & curve, double t1, double t2, FUNCTION f)
    {
        this->depth = 0;

        IntPoint from;
        from.t = t1;
        from.lc = f(curve, from.t);
        from.p = 0.0;
        this->pts.push_back(from);

        IntPoint to;
        to.t = t2;
        to.lc = f(curve, to.t);

        integrate_recursively(curve, from, to, f);
        this->val = this->pts.back().p;
    }

private:
    [[nodiscard]] double
    trapezoidal(const IntPoint & p1, const IntPoint & p2)
    {
        return 0.5 * (p1.lc + p2.lc) * (p2.t - p1.t);
    }

    template <typename FUNCTION>
    void
    integrate_recursively(const GeomCurve & curve, IntPoint & from, IntPoint & to, FUNCTION f)
    {
        this->depth++;

        IntPoint mid;
        mid.t = 0.5 * (from.t + to.t);
        mid.lc = f(curve, mid.t);

        double const val1 = trapezoidal(from, to);
        double const val2 = trapezoidal(from, mid);
        double const val3 = trapezoidal(mid, to);
        double const err = std::abs(val1 - val2 - val3);

        if (((err < this->precision) && (this->depth > 6)) || (this->depth > 25)) {
            IntPoint p1 = this->pts.back();
            mid.p = p1.p + val2;
            this->pts.push_back(mid);

            p1 = this->pts.back();
            to.p = p1.p + val3;
            this->pts.push_back(to);
        }
        else {
            integrate_recursively(curve, from, mid, f);
            integrate_recursively(curve, mid, to, f);
        }

        this->depth--;
    }

    /// Value of the integral
    double val;
    /// integration points
    std::vector<IntPoint> pts;
    /// resursive depth during integration
    int depth;
    ///
    double precision = 1e-8;
};

} // namespace krado
