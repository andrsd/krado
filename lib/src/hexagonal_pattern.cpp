// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/hexagonal_pattern.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/axis1.h"
#include "krado/range.h"
#include <array>

namespace krado {

constexpr int N_CORNERS = 6;
constexpr int N_SIDES = 6;
constexpr double DEG60 = M_PI / 3.;

HexagonalPattern::HexagonalPattern(const std::vector<Point> & points,
                                   const Axis2 & center,
                                   double flat_to_flat,
                                   int side_segs) :
    Pattern(points),
    center_(center),
    flat_to_flat_(flat_to_flat),
    num_side_segs_(side_segs)
{
}

Point
HexagonalPattern::center() const
{
    return this->center_.location();
}

double
HexagonalPattern::flat_to_flat() const
{
    return this->flat_to_flat_;
}

int
HexagonalPattern::num_side_segments() const
{
    return this->num_side_segs_;
}

HexagonalPattern
HexagonalPattern::create(const Axis2 & center, double flat_to_flat, int side_segs)
{
    auto ctr_pt = center.location();
    double radius = flat_to_flat / std::sqrt(3.);

    auto x_vec = radius * center.x_direction();
    Axis1 ctr_ax1(ctr_pt, center.direction());

    // find corners
    std::array<Point, N_CORNERS> corners;
    for (auto i : make_range(N_CORNERS)) {
        double angle = i * DEG60;
        auto v = x_vec.rotated(ctr_ax1, angle);
        auto pt = ctr_pt + v;
        corners[i] = pt;
    }

    // build sides
    std::vector<Point> points;
    points.reserve(N_SIDES);
    for (auto s : make_range(N_SIDES)) {
        Vector side = corners[(s + 1) % N_SIDES] - corners[s];
        Vector side_dir = side.normalized();
        double ds = side.magnitude() / side_segs;
        for (auto i : make_range(side_segs)) {
            auto pt = corners[s] + i * ds * side_dir;
            points.emplace_back(pt);
        }
    }

    return { points, center, flat_to_flat, side_segs };
}

} // namespace krado
