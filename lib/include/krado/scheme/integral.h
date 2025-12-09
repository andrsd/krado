// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include <cstddef>
#include <vector>

namespace krado {

class Integral {
private:
    /// Integration point
    struct Pt {
        /// Local coordinate of the point
        double t;
        /// x'(t)/h(x(t))
        double lc;
        /// p is the value of the primitive
        double p;
    };

public:
    Integral() : precision_(1e-8) {}

    template <typename FN>
    void
    integrate(const GeomCurve & curve, FN fn)
    {
        auto [t_lo, t_hi] = curve.param_range();
        Pt from { t_lo, fn(t_lo), 0. };
        this->points_.push_back(from);

        Pt to { t_hi, fn(t_lo), 0. };
        recursive_integrate(from, to, fn, 0);
    }

    std::size_t
    num_point() const
    {
        return this->points_.size();
    }

    const Pt &
    point(std::size_t idx) const
    {
        return this->points_[idx];
    }

private:
    template <typename FN>
    void
    recursive_integrate(const Pt & from, Pt & to, FN fn, int depth)
    {
        auto t_mid = 0.5 * (from.t + to.t);
        Pt mid { t_mid, fn(t_mid), 0. };

        const auto val1 = trapezoidal(from, to);
        const auto val2 = trapezoidal(from, mid);
        const auto val3 = trapezoidal(mid, to);
        const auto err = std::abs(val1 - val2 - val3);

        if (((err < this->precision_) && (depth > 6)) || (depth > 25)) {
            auto & p1 = this->points_.back();
            mid.p = p1.p + val2;
            this->points_.push_back(mid);

            to.p = mid.p + val3;
            this->points_.push_back(to);
        }
        else {
            recursive_integrate(from, mid, fn, depth + 1);
            recursive_integrate(mid, to, fn, depth + 1);
        }
    }

    /// Trapezoidal integration rule from `pt1` to `pt2`
    static double
    trapezoidal(const Pt & pt1, const Pt & pt2)
    {
        return 0.5 * (pt1.lc + pt2.lc) * (pt2.t - pt1.t);
    }

    double precision_;
    std::vector<Pt> points_;
};

} // namespace krado
