// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include <array>

namespace krado {

class UVParam;
class Point;
class Vector;

/// Sign function
///
/// @tparam T C++ type
/// @param value Value to check
/// @return -1 for negative values, 0 for zero, and 1 for positive values
template <typename T>
inline int
sign(T const & value)
{
    return (T(0) < value) - (value < T(0));
}

/// Solve linear system of 2x2
///
/// @param mat Matrix
/// @param b Right-hand side
/// @param Solution if invertible, `nullopt` if singular
Optional<std::array<double, 2>> sys2x2(std::array<std::array<double, 2>, 2> mat,
                                       std::array<double, 2> b);

/// Compute normal from 3 points in 3D space (using left-hand rule)
///
/// @param a First point
/// @param b Second point
/// @param c Third point
/// @return Normal
Vector normal(Point p0, Point p1, Point p2);

/// Compute normal from 3 points in 2D parameterical space (using left-hand rule)
///
/// @param a First point
/// @param b Second point
/// @param c Third point
/// @return Normal
Vector normal(UVParam uv0, UVParam uv1, UVParam uv2);

} // namespace krado
