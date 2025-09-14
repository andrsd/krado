// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include "Standard_TypeDef.hxx"
#include <tuple>
#include <map>

namespace krado {

class GeomVolume;
class GeomSurface;
class GeomCurve;
class GeomShell;
class Mesh;

/// Split a curve at a given parameter
///
/// @param curve Curve to split
/// @param split_param Parameter to split the curve at (must be between the curve's parameter range)
/// @return Tuple of two curves
[[nodiscard]] std::tuple<GeomCurve, GeomCurve> split_curve(const GeomCurve & curve,
                                                           Standard_Real split_param);

/// Imprint a curve on a surface
///
/// @param surface Surface to imprint the curve on
/// @param curve Curve to imprint
/// @return Imprinted shell
[[nodiscard]] GeomShell imprint(const GeomSurface & surface, const GeomCurve & curve);

/// Imprint a curve on a volume
///
/// @param volume Volume to imprint the curve on
/// @param curve Curve to imprint
/// @return Imprinted volume
[[nodiscard]] GeomVolume imprint(const GeomVolume & volume, const GeomCurve & curve);

/// Imprint a curve on a volume
///
/// @param volume Volume to imprint the curve on
/// @param curve Curve to imprint
/// @return Imprinted volume
[[nodiscard]] GeomVolume imprint(const GeomVolume & volume, const GeomVolume & imp_volume);

/// Compute volume of blocks
///
/// @param mesh Mesh to compute volumes of blocks
/// @return Computed
std::map<marker_t, double> compute_volume(const Mesh & mesh);

} // namespace krado
