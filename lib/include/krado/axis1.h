// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include "gp_Ax1.hxx"

namespace krado {

class Axis1 {
public:
    /// @param pt `Point` the location point
    /// @param direction `Vector` the direction of the axis
    Axis1(const Point & pt, const Vector & dir);

    /// Get location
    ///
    /// @return Location
    Point location() const;

    /// Get direction
    ///
    /// @return Direction
    Vector direction() const;

    bool is_equal(const Axis1 & other, double tol) const;

private:
    gp_Ax1 ax1_;
};

} // namespace krado
