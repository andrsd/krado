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
class Axis1;
class Wire;
class Plane;

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

/// Mirror a shape about an axis
///
/// @param shape Shape to mirror
/// @param axis Axis to mirror about
/// @return Resulting shape
GeomShape mirror(const GeomShape & shape, const Axis1 & axis);

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

/// Fillet
///
/// @param shape Shape to fillet
/// @param curves List of curves to fillet
/// @param radius Radius of the fillet
/// @return Resulting shape
GeomShape fillet(const GeomShape & shape, const std::vector<GeomCurve> & curves, double radius);

/// Hollow the shape
///
/// @param shape Shape to hollow
/// @param faces_to_remove Faces to open
/// @param thickness Desired wall thickness
/// @param tolerance Tolerance
/// @return Hollowed out shape
GeomShape hollow(const GeomShape & shape,
                 const std::vector<GeomSurface> & faces_to_remove,
                 double thickness,
                 double tolerance);

/// Extrude a shape
///
/// @param shape Shape to extrude
/// @param vec Vector of extrusion
/// @return Resulting shape
GeomShape extrude(const GeomShape & shape, const Vector & vec);

/// Revolve shape about an axis
///
/// @param shape Shape to revolve
/// @param axis Axis to revolve about
/// @param angle Angle to revolve
/// @return Resulting shape
GeomShape revolve(const GeomShape & shape, const Axis1 & axis, double angle = 2. * M_PI);

/// Rotate a shape about an axis
///
/// @param shape Shape to rotate
/// @param axis Axis to rotate about
/// @param angle Rotation angle (in radians)
/// @return Resulting `Shape`
GeomShape rotate(const GeomShape & shape, const Axis1 & axis, double angle);

/// Compute section between a shape and a plane
///
/// @param shape Shape
/// @param plane Plane
/// @return Wire that forms the computed section
Wire section(const GeomShape & shape, const Plane & plane);

/// Taper-adding transformations on a shape. The resulting shape is constructed by defining one face
/// to be tapered after another one, as well as the geometric properties of their tapered
/// transformation. Each tapered transformation is propagated along the series of faces which are
/// tangential to one another and which contains the face to be tapered.
///
/// @param shape Shape to taper
/// @param pln Neutral plane
/// @param faces Faces to taper
/// @param angle Tapering angle
/// @return Tapered shape
GeomShape draft(const GeomShape & shape,
                const Plane & pln,
                const std::vector<GeomSurface> & faces,
                double angle);

/// Make cylindrical hole on a shape
///
/// @param shape Shape on which hole will be performed
/// @param axis Axis of the hole
/// @param diameter Diameter of the hole
GeomShape hole(const GeomShape & shape, const Axis1 & axis, double diameter);

/// Make cylindrical hole on a shape with specified depth
///
/// @param shape Shape on which hole will be performed
/// @param axis Axis of the hole
/// @param diameter Diameter of the hole
/// @param length Length of the hole
GeomShape hole(const GeomShape & shape, const Axis1 & axis, double diameter, double length);

/// Constructs a pipe by sweeping the `profile` along the `spine`
///
/// @param profile Profile to sweep
/// @param spine Sweep path
/// @return Resulting shape
GeomShape sweep(const GeomShape & profile, const Wire & spine);

/// Sew faces into a shell
///
/// @param faces Faces to sew
/// @param tol Tolerance
/// @return Resulting shell
GeomShape sew(const std::vector<GeomShape> & faces, double tol = 1e-6);

} // namespace krado
