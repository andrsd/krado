// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/hexagonal_pattern.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/axis1.h"
#include <array>

namespace krado {

constexpr int N_CORNERS = 6;
constexpr int N_SIDES = 6;
constexpr double DEG60 = M_PI / 3.;

HexagonalPattern::HexagonalPattern(const Axis2 & center, double flat_to_flat, int side_segs) :
    Pattern(),
    center_(center),
    flat_to_flat_(flat_to_flat),
    num_side_segs_(side_segs)
{
    auto ctr_pt = center.location();
    double radius = flat_to_flat / std::sqrt(3.);

    auto x_vec = radius * center.x_direction();
    Axis1 ctr_ax1(ctr_pt, center.direction());

    // find corners
    std::array<Point, N_CORNERS> corners;
    for (int i = 0; i < N_CORNERS; ++i) {
        double angle = i * DEG60;
        auto v = x_vec.rotated(ctr_ax1, angle);
        auto pt = ctr_pt + v;
        corners[i] = pt;
    }

    // build sides
    std::vector<Point> points;
    for (int s = 0; s < N_SIDES; ++s) {
        Vector side = corners[(s + 1) % N_SIDES] - corners[s];
        Vector side_dir = side.normalized();
        double ds = side.magnitude() / this->num_side_segs_;
        for (int i = 0; i < this->num_side_segs_; ++i) {
            auto pt = corners[s] + i * ds * side_dir;
            points.emplace_back(pt);
        }
    }

    set_points(std::move(points));
}

double
HexagonalPattern::flat_to_flat() const
{
    return this->flat_to_flat_;
}

} // namespace krado
