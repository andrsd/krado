// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/linear_pattern.h"
#include "krado/vector.h"
#include "krado/range.h"

namespace krado {

LinearPattern::LinearPattern(const std::vector<Point> & points,
                             const Axis2 & origin,
                             int nx,
                             int ny,
                             double dx,
                             double dy) :
    Pattern(points),
    ax2_(origin),
    nx_(nx),
    dx_(dx),
    ny_(ny),
    dy_(dy)
{
}

double
LinearPattern::nx() const
{
    return this->nx_;
}

double
LinearPattern::ny() const
{
    return this->ny_;
}

double
LinearPattern::dx() const
{
    return this->dx_;
}

double
LinearPattern::dy() const
{
    return this->dy_;
}

LinearPattern
LinearPattern::create(const Axis2 & ax2, int nx, double dx)
{
    std::vector<Point> points;
    points.reserve(nx);
    auto origin = ax2.location();
    auto x_vec = Vector(ax2.x_direction());
    for (auto i : make_range(nx)) {
        auto x_ofst = i * dx * x_vec;
        auto pt = origin + x_ofst;
        points.emplace_back(pt);
    }
    return { points, ax2, nx, 0, dx, 0. };
}

LinearPattern
LinearPattern::create(const Axis2 & ax2, int nx, int ny, double dx, double dy)
{
    std::vector<Point> points;
    points.reserve(nx * ny);
    auto origin = ax2.location();
    auto x_vec = Vector(ax2.x_direction());
    auto y_vec = Vector(ax2.y_direction());
    for (auto j : make_range(ny)) {
        auto y_ofst = j * dy * y_vec;
        for (auto i : make_range(nx)) {
            auto x_ofst = i * dx * x_vec;
            auto pt = origin + x_ofst + y_ofst;
            points.emplace_back(pt);
        }
    }
    return { points, ax2, nx, ny, dx, dy };
}

} // namespace krado
