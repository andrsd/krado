// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "Standard_TypeDef.hxx"
#include <tuple>

namespace krado {

class GeomCurve;

/// Split a curve at a given parameter
///
/// @param curve Curve to split
/// @param split_param Parameter to split the curve at (must be between the curve's parameter range)
/// @return Tuple of two curves
[[nodiscard]] std::tuple<GeomCurve, GeomCurve> split_curve(const GeomCurve & curve,
                                                           Standard_Real split_param);

} // namespace krado
