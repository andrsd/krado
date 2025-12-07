// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/polygon.h"
#include "krado/exception.h"
#include "krado/axis1.h"
#include "krado/axis2.h"
#include "krado/vector.h"
#include "krado/occ.h"

namespace krado {

Polygon::Polygon(const TopoDS_Wire & wire) : Wire(wire) {}

std::vector<Point>
Polygon::build_points(const Axis2 & ax2, const Vector & start_vec, int n_sides)
{
    std::vector<Point> points;
    auto ctr_pt = ax2.location();
    Axis1 ctr_ax1(ctr_pt, ax2.direction());
    auto dangle = 2 * M_PI / n_sides;
    for (int i = 0; i < n_sides; i++) {
        auto vec = start_vec.rotated(ctr_ax1, i * dangle);
        auto pt1 = ctr_pt + vec;
        points.emplace_back(pt1);
    }
    return points;
}

BRepBuilderAPI_MakePolygon
Polygon::build_polygon(const std::vector<Point> & points, bool closed)
{
    BRepBuilderAPI_MakePolygon polygon;
    for (auto & pt : points)
        polygon.Add(occ::to_pnt(pt));
    if (closed)
        polygon.Close();
    polygon.Build();
    if (!polygon.IsDone())
        throw Exception("Polygon was not created");
    return polygon;
}

Polygon
Polygon::create(const std::vector<Point> & points, bool closed)
{
    if (points.size() < 3)
        throw Exception("Polygon needs at least 3 points");
    auto polygon = Polygon::build_polygon(points, closed);
    return Polygon(polygon.Wire());
}

} // namespace krado
