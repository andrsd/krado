// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "krado/point.h"
#include "formo/vector.h"
#include "gp_Circ.hxx"

namespace krado {

class Axis2;

/// Describes a circle in 3D space. A circle is defined by its radius and positioned in space with a
/// coordinate system.
class Circle : public GeomCurve {
public:
    /// Get circle area
    ///
    /// @return Area of the circle
    double area() const;

    /// Get radius
    ///
    /// @return Radius
    double radius() const;

    /// Get circle location
    Point location() const;

private:
    Circle(const TopoDS_Edge &, const gp_Circ & circ);

    gp_Circ circ_;

public:
    /// Construct a circle from an origin and a radius.
    ///
    /// @param origin Center point
    /// @param radius Radius
    static Circle create(const Axis2 & origin, double radius);

    /// Construct a circle from a center point and a radius.
    ///
    /// @param center Center point
    /// @param radius Radius
    /// @param norm Normal of the plane
    static Circle
    create(const Point & center, double radius, const Vector & normal = Vector(0., 0., 1.));

    /// Construct a circle from a center point and another point
    ///
    /// @param center Center point
    /// @param pt Point that is part of the circle
    /// @param normal Normal of the plane
    static Circle
    create(const Point & center, const Point & pt, const Vector & normal = Vector(0., 0., 1.));

    /// Construct a circle from three points
    ///
    /// @param pt1 First point
    /// @param pt2 Second point
    /// @param pt3 Third point
    static Circle create(const Point & pt1, const Point & pt2, const Point & pt3);
};

} // namespace krado
