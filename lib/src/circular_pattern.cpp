// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/circular_pattern.h"
#include "krado/vector.h"
#include "krado/axis1.h"
#include "krado/range.h"

namespace krado {

CircularPattern::CircularPattern(const std::vector<Point> & points,
                                 const Axis2 & center,
                                 double radius,
                                 int divisions) :
    Pattern(points),
    center_(center),
    radius_(radius),
    divs_(divisions)
{
}

Point
CircularPattern::center() const
{
    return this->center_.location();
}

double
CircularPattern::radius() const
{
    return this->radius_;
}

int
CircularPattern::divisions() const
{
    return this->divs_;
}

CircularPattern
CircularPattern::create(const Axis2 & center, double radius, int divisions, double start_angle)
{
    std::vector<Point> points;
    auto ctr = center.location();
    auto x_vec = radius * Vector(center.x_direction());
    x_vec.rotate(center.axis(), start_angle);
    Axis1 ctr_ax1(ctr, center.direction());
    double dangle = 2. * M_PI / divisions;
    for (auto i : make_range(divisions)) {
        double angle = i * dangle;
        auto v = x_vec.rotated(ctr_ax1, angle);
        auto pt = ctr + v;
        points.emplace_back(pt);
    }
    return { points, center, radius, divisions };
}

} // namespace krado
