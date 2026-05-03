// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"

namespace krado {

class UVParam;
class Point;

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

/// Compute circum center in (u, v) parameter space
///
/// @param p1 Point one
/// @param p2 Point two
/// @param p3 Point three
/// @return Circum center, nullopt if points are colinear
Optional<UVParam> circum_center(UVParam p1, UVParam p2, UVParam p3);

/// Compute circum center
///
/// @param p1 Point one
/// @param p2 Point two
/// @param p3 Point three
/// @return Circum center
Point circum_center(Point p1, Point p2, Point p3);

} // namespace krado
