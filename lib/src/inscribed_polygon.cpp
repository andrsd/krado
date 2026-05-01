// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/inscribed_polygon.h"
#include "krado/axis2.h"
#include "krado/point.h"
#include "krado/polygon.h"
#include "krado/vector.h"
#include "krado/exception.h"
#include "BRepBuilderAPI_MakePolygon.hxx"

namespace krado {

InscribedPolygon::InscribedPolygon(const TopoDS_Wire & wire) : Polygon(wire) {}

InscribedPolygon
InscribedPolygon::create(const Axis2 & ax2, double radius, int n_sides)
{
    if (n_sides < 3)
        throw Exception("CircumscribedPolygon needs at least 3 sides");

    auto points = build_points(ax2, radius * ax2.x_direction(), n_sides);
    InscribedPolygon polygon(build_polygon(points, true));
    polygon.n_sides_ = n_sides;
    polygon.radius_ = radius;
    return polygon;
}

InscribedPolygon
InscribedPolygon::create(const Axis2 & ax2, const Point & pt1, int n_sides)
{
    if (n_sides < 3)
        throw Exception("CircumscribedPolygon needs at least 3 sides");

    auto vec = pt1 - ax2.location();
    auto radius = vec.magnitude();
    auto points = build_points(ax2, vec, n_sides);
    InscribedPolygon polygon(build_polygon(points, true));
    polygon.n_sides_ = n_sides;
    polygon.radius_ = radius;
    return polygon;
}

} // namespace krado
