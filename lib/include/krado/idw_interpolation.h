// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"

namespace krado {

/// Inverse distance interpolation
///
/// This class provides a simple inverse distance interpolation in 3D space.
/// The interpolation is based on the formula:
///
/// \f[
/// f(\mathbf{x}) = \frac{\sum_{i=1}^{n} w_i(\mathbf{x}) f_i}{\sum_{i=1}^{n} w_i(\mathbf{x})}
/// \f]
///
/// where \f$f_i\f$ are the values of the points, \f$w_i(\mathbf{x})\f$ are the weights
/// and \f$\mathbf{x}\f$ is the point where we want to interpolate.
///
/// The weights are computed as:
///
/// \f[
/// w_i(\mathbf{x}) = \frac{1}{\|\mathbf{x} - \mathbf{x}_i\|^p}
/// \f]
///
/// where \f$\mathbf{x}_i\f$ are the points and \f$p\f$ is the power parameter.
/// The power parameter is set to 2 by default.
///
class IDWInterpolation {
public:
    /// Construct empty interpolation
    IDWInterpolation();

    /// Construct interpolation with given points and weights
    ///
    /// @param pnts Points in 3D space
    /// @param weights Weights for each point
    IDWInterpolation(const std::vector<Point> & pnts, const std::vector<double> & weights);

    /// Add a point to the interpolation
    ///
    /// @param pt Point in 3D space
    /// @param w Weight for the point
    void add_point(const Point & pt, double w);

    /// Sample the function at a given point
    ///
    /// @param pt Point in 3D space
    /// @param power Power parameter
    /// @return Interpolated value
    double sample(const Point & pt, double power = 2.) const;

private:
    /// Point sin 3D space
    std::vector<Point> pts;
    /// Weights for each point
    std::vector<double> weights;
};

} // namespace krado
