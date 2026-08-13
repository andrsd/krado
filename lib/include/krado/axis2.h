// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "gp_Ax2.hxx"

namespace krado {

class Vector;
class Axis1;

class Axis2 {
public:
    explicit Axis2(const gp_Ax2 & ax2);

    Axis2(Point pt, Vector direction);

    /// Creates an axis placement with an origin `pt` such that:
    /// - `n` is the Direction, and
    /// - the "X direction" is normal to `n`, in the plane defined by the vectors (`n`, `v_x`):
    ///   "X Direction" = (n ^ v_x) ^ n,
    ///
    /// Raises `Exception` if N and Vx are parallel (same or opposite orientation).
    Axis2(Point pt, Vector n, Vector v_x);

    /// Get location
    ///
    /// @return Location
    [[nodiscard]] Point location() const;

    /// Returns the main axis
    ///
    /// @return The main location point and the main direction
    [[nodiscard]] Axis1 axis() const;

    /// Get direction
    ///
    /// @return Direction
    [[nodiscard]] Vector direction() const;

    /// Get x-direction
    ///
    /// @return x-direction
    [[nodiscard]] Vector x_direction() const;

    /// Get y-direction
    ///
    /// @return x-direction
    [[nodiscard]] Vector y_direction() const;

    operator gp_Ax2() const;

private:
    gp_Ax2 ax2_;
};

} // namespace krado
