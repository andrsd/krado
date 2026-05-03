// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/numerics.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/uv_param.h"

namespace krado {

Optional<UVParam>
circum_center(UVParam p1, UVParam p2, UVParam p3)
{
    const double x1 = p1.u;
    const double x2 = p2.u;
    const double x3 = p3.u;
    const double y1 = p1.v;
    const double y2 = p2.v;
    const double y3 = p3.v;

    auto d = 2. * (y1 * (x2 - x3) + y2 * (x3 - x1) + y3 * (x1 - x2));
    if (d == 0.0)
        return std::nullopt;

    auto a1 = x1 * x1 + y1 * y1;
    auto a2 = x2 * x2 + y2 * y2;
    auto a3 = x3 * x3 + y3 * y3;

    return UVParam(((a1 * (y3 - y2) + a2 * (y1 - y3) + a3 * (y2 - y1)) / d),
                   ((a1 * (x2 - x3) + a2 * (x3 - x1) + a3 * (x1 - x2)) / d));
}

Point
circum_center(Point p1, Point p2, Point p3)
{
    auto v1 = p2 - p1;
    auto v2 = p3 - p1;
    auto vx = p2 - p1;
    auto vy = p3 - p1;
    auto vz = cross_product(vx, vy);
    vy = cross_product(vz, vx);
    vx.normalize();
    vy.normalize();
    vz.normalize();

    UVParam p1P(0., 0.);
    UVParam p2P(dot_product(v1, vx), dot_product(v1, vy));
    UVParam p3P(dot_product(v2, vx), dot_product(v2, vy));
    auto resP = circum_center(p1P, p2P, p3P);
    if (resP.has_value()) {
        auto r = resP.value();
        auto center = p1 + r.u * vx + r.v * vy;
        return center;
    }
    else {
        auto mn = std::numeric_limits<double>::lowest();
        return Point(mn, mn, mn);
    }
}

} // namespace krado
