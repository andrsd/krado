// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class GeomVolume;
class GeomSurface;
class GeomCurve;
class GeomShell;

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

} // namespace krado
