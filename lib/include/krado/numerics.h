// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/uv_param.h"
#include <Eigen/Eigen>

namespace krado {

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

} // namespace krado
