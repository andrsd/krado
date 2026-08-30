// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/pattern.h"
#include "krado/axis2.h"

namespace krado {

/// Circular pattern
class CircularPattern : public Pattern {
    CircularPattern(const std::vector<Point> & points,
                    const Axis2 & center,
                    double radius,
                    int divisions);

public:
    /// Center
    [[nodiscard]] Point center() const;

    /// Get radius
    [[nodiscard]] double radius() const;

    /// Get divisions
    [[nodiscard]] int divisions() const;

private:
    /// Center of the pattern
    Axis2 center_;
    /// Radius
    double radius_;
    /// number of divisions
    int divs_;

public:
    /// Create circular pattern over full circle
    ///
    /// @param center Center of the pattern
    /// @param radius Radius of the pattern
    /// @param divisions Number of segments around the circle
    /// @return Circular pattern
    static CircularPattern
    create(const Axis2 & center, double radius, int divisions, double start_angle = 0.);
};

} // namespace krado
