// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class GeomSurface;
class GeomCurve;
class GeomShell;

/// Imprint a curve on a surface
///
/// @param surface Surface to imprint the curve on
/// @param curve Curve to imprint
/// @return Imprinted shell
GeomShell imprint(const GeomSurface & surface, const GeomCurve & curve);

} // namespace krado
