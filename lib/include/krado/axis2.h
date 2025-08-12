// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include "krado/axis1.h"
#include "gp_Ax2.hxx"

namespace krado {

class Axis2 {
public:
    Axis2(const Point & pt, const Vector & direction);

    /// Creates an axis placement with an origin `pt` such that:
    /// - `n` is the Direction, and
    /// - the "X direction" is normal to `n`, in the plane defined by the vectors (`n`, `v_x`):
    ///   "X Direction" = (n ^ v_x) ^ n,
    ///
    /// Raises `Exception` if N and Vx are parallel (same or opposite orientation).
    Axis2(const Point & pt, const Vector & n, const Vector & v_x);

    /// Get location
    ///
    /// @return Location
    Point location() const;

    /// Returns the main axis
    ///
    /// @return The main location point and the main direction
    Axis1 axis() const;

    /// Get direction
    ///
    /// @return Direction
    Vector direction() const;

    /// Get x-direction
    ///
    /// @return x-direction
    Vector x_direction() const;

    /// Get y-direction
    ///
    /// @return x-direction
    Vector y_direction() const;

private:
    gp_Ax2 ax2_;
};

} // namespace krado
