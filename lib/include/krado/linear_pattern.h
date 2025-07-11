// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/pattern.h"
#include "krado/axis2.h"

namespace krado {

/// Linear pattern
class LinearPattern : public Pattern {
public:
    LinearPattern(const Axis2 & origin, int nx, double dx);
    LinearPattern(const Axis2 & origin, int nx, int ny, double dx, double dy);

    /// Number of points in x-direction
    double nx() const;

    /// Number of points in y-direction
    double ny() const;

    /// Distance between 2 points in x-direction
    double dx() const;

    /// Distance between 2 points in y-direction
    double dy() const;

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
};

} // namespace krado
