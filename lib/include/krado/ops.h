// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
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
class Point;
class Vector;

/// Translate a shape
///
/// @param shape Shape to translate
/// @param v Translation vector
/// @return Translated shape
GeomShape translate(const GeomShape & shape, const Vector & v);

/// Translate a shape
///
/// @param shape Shape to translate
/// @param p1 Point one of the translation vector
/// @param p2 Point two of the translation vector
/// @return Translated shape
GeomShape translate(const GeomShape & shape, const Point & p1, const Point & p2);

/// Scale a shape
///
/// @param shape Shape to scale
/// @param s Scale factor
/// @return Scaled shape
GeomShape scale(const GeomShape & shape, double s);

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
/// @return Computed volumes
std::map<marker_t, double> compute_volume(const Mesh & mesh);

/// Combine mesh part into one large mesh
///
/// @param parts Mesh parts to combine
/// @return Resulting mesh
Mesh combine(const std::vector<Mesh> & parts);

/// Fuse 2 shapes
///
/// @param shape Shape
/// @param tool Tool
/// @return Resulting fused shape
GeomShape fuse(const GeomShape & shape, const GeomShape & tool, bool simplify = true);

/// Fuse multiple shapes
///
/// @param tools Shapes to fuse
/// @return Resulting fused shape
GeomShape fuse(const std::vector<GeomShape> & tools, bool simplify = true);

/// Cut a shape with a tool
///
/// @param shape Shape to cut
/// @param tool Cutting tool
/// @return Resulting shape
GeomShape cut(const GeomShape & shape, const GeomShape & tool);

/// Intersect 2 shapes
///
/// @param shape Shape
/// @param tool Tool
/// @return Resulting shape
GeomShape intersect(const GeomShape & shape, const GeomShape & tool);

} // namespace krado
