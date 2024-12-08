// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/idw_interpolation.h"
#include "krado/exception.h"
#include "krado/vector.h"

namespace krado {

IDWInterpolation::IDWInterpolation() {}

IDWInterpolation::IDWInterpolation(const std::vector<Point> & pnts,
                                   const std::vector<double> & weights) :
    pts(pnts),
    weights(weights)
{
    if (pnts.size() != weights.size())
        throw Exception("Number of points and weights must be the same");
}

void
IDWInterpolation::add_point(const Point & pt, double w)
{
    this->pts.push_back(pt);
    this->weights.push_back(w);
}

double
IDWInterpolation::sample(const Point & pt, double power) const
{
    if (this->pts.empty())
        throw Exception("No points for interpolation");

    double weighted_sum = 0.0;
    double weight_total = 0.0;
    for (std::size_t i = 0; i < this->pts.size(); ++i) {
        auto distance = (this->pts[i] - pt).norm();
        if (distance == 0.0)
            return this->weights[i];

        double weight = 1.0 / std::pow(distance, power);
        weighted_sum += weight * this->weights[i];
        weight_total += weight;
    }
    return weighted_sum / weight_total;
}

} // namespace krado
