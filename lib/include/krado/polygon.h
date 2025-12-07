// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/wire.h"
#include "BRepBuilderAPI_MakePolygon.hxx"
#include <vector>

namespace krado {

class Axis2;
class Point;
class Vector;

class Polygon : public Wire {
protected:
    Polygon(const TopoDS_Wire & wire);

public:
    /// Create a polygon from set of points
    ///
    /// @param points List of points
    /// @param closed Flag that indicates if the last point will be connect to the first point,
    ///        i.e. creating closed or open polygon.
    static Polygon create(const std::vector<Point> & points, bool closed = true);

protected:
    static std::vector<Point> build_points(const Axis2 & ax2, const Vector & vec, int n_sides);
    static BRepBuilderAPI_MakePolygon build_polygon(const std::vector<Point> & points, bool closed);
};

} // namespace krado
