// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/polygon.h"

namespace krado {

/// A class representing a regular polygon circumscribed around a circle. That is, the polygon is
/// constructed such that all of its sides touch a circle.
class CircumscribedPolygon : public Polygon {
private:
    CircumscribedPolygon(const TopoDS_Wire & wire);

private:
    /// Radius of the circle
    double radius_;
    /// Number of sides
    int n_sides_;

public:
    /// Create circumscribed polygon from radius and number of sides
    ///
    /// @param radius Radius if the circle
    /// @param n_sides Number of sides (must be 3 or more)
    static CircumscribedPolygon create(const Axis2 & ax2, double radius, int n_sides);

    /// Create circumscribed polygon from a point and number of sides
    ///
    /// @param pt1 Point defining the circle and the first point of the polygon
    /// @param n_sides Number of sides (must be 3 or more)
    static CircumscribedPolygon create(const Axis2 & ax2, const Point & pt1, int n_sides);
};

} // namespace krado
