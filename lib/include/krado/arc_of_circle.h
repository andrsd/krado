// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/circle.h"
#include "Geom_TrimmedCurve.hxx"

namespace krado {

/// Describes an arc of a circle in 3D space.
class ArcOfCircle : public GeomCurve {
public:
    /// Get the start point of the curve
    Point start_point() const;

    /// Get the end point of the curve
    Point end_point() const;

private:
    ArcOfCircle(const TopoDS_Edge & edge, Handle(Geom_TrimmedCurve) arc);

    Handle(Geom_TrimmedCurve) arc_;

public:
    /// Construct an arc of a circle from trhee points
    ///
    /// @param pt1 First point
    /// @param pt2 Second point
    /// @param pt3 Third point
    static ArcOfCircle create(const Point & pt1, const Point & pt2, const Point & pt3);

    /// Construct an arc of a circle from a circle and 2 points on the circle.
    ///
    /// @param circ Circle
    /// @param pt1 First point
    /// @param pt2 Second point
    /// @param sense Sense of the circle
    static ArcOfCircle
    create(const class Circle & circ, const Point & pt1, const Point & pt2, bool sense = true);

    /// Construct an arc of a circle from a point, tangent at the point, and another point.
    ///
    /// @param pt1 First point
    /// @param tangent Tangent at point `pt1`
    /// @param pt2 Second point
    static ArcOfCircle create(const Point & pt1, const Vector & tangent, const Point & pt2);
};

} // namespace krado
