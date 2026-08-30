// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/pattern.h"
#include "krado/axis2.h"

namespace krado {

/// Linear pattern
class LinearPattern : public Pattern {
    LinearPattern(const std::vector<Point> & points,
                  const Axis2 & origin,
                  int nx,
                  int ny,
                  double dx,
                  double dy);

public:
    /// Number of points in x-direction
    [[nodiscard]] double nx() const;

    /// Number of points in y-direction
    [[nodiscard]] double ny() const;

    /// Distance between 2 points in x-direction
    [[nodiscard]] double dx() const;

    /// Distance between 2 points in y-direction
    [[nodiscard]] double dy() const;

private:
    /// Origin of the pattern
    Axis2 ax2_;
    /// number of points in x-direction
    int nx_;
    ///
    double dx_;
    /// number of points in y-direction
    int ny_;
    ///
    double dy_;

public:
    /// Create linear pattern in 1 direction
    ///
    /// @param origin Location where the pattern starts
    /// @param nx Number of horizontal "points"
    /// @param dx Distance between "points"
    /// @return Linear pattern
    static LinearPattern create(const Axis2 & origin, int nx, double dx);

    /// Create linear pattern in 2 directions
    ///
    /// @param origin Location where the pattern starts
    /// @param nx Number of horizontal "points"
    /// @param ny Number of vertical "points"
    /// @param dx Distance between horizontal "points"
    /// @param dy Distance between vertical "points"
    /// @return Linear pattern
    static LinearPattern create(const Axis2 & origin, int nx, int ny, double dx, double dy);
};

} // namespace krado
