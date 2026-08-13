// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/circumscribed_polygon.h"
#include "krado/axis2.h"
#include "krado/point.h"
#include "krado/polygon.h"
#include "krado/vector.h"
#include "krado/exception.h"
#include "BRepBuilderAPI_MakePolygon.hxx"

namespace krado {

CircumscribedPolygon::CircumscribedPolygon(const TopoDS_Wire & wire, double radius, int n_sides) :
    Polygon(wire),
    radius_(radius),
    n_sides_(n_sides)
{
}

double
CircumscribedPolygon::radius() const
{
    return this->radius_;
}

int
CircumscribedPolygon::num_sides() const
{
    return this->n_sides_;
}

CircumscribedPolygon
CircumscribedPolygon::create(const Axis2 & ax2, double radius, int n_sides)
{
    if (n_sides < 3)
        throw Exception("CircumscribedPolygon needs at least 3 sides");

    auto r_out = radius / std::cos(M_PI / n_sides);
    auto points = build_points(ax2, r_out * ax2.x_direction(), n_sides);
    return { build_polygon(points, true), radius, n_sides };
}

CircumscribedPolygon
CircumscribedPolygon::create(const Axis2 & ax2, Point pt1, int n_sides)
{
    if (n_sides < 3)
        throw Exception("CircumscribedPolygon needs at least 3 sides");

    auto vec = pt1 - ax2.location();
    auto radius = vec.magnitude();
    auto r_out = radius / std::cos(M_PI / n_sides);
    auto points = build_points(ax2, r_out * vec.normalized(), n_sides);
    return { build_polygon(points, true), radius, n_sides };
}

} // namespace krado
