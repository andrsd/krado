// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/geom_vertex.h"
#include "TopoDS_Edge.hxx"
#include "Geom_Curve.hxx"
#include "GeomAPI_ProjectPointOnCurve.hxx"

namespace krado {

class GeomModel;
class GeomSurface;

class GeomCurve : public GeomShape {
public:
    enum CurveType { Line, Circle, BSpline, Bezier, Unknown };

    explicit GeomCurve(const TopoDS_Edge & edge);

    /// Get curve type
    ///
    /// @return Curve type
    [[nodiscard]] CurveType type() const;

    /// Check if the edge is degenerated
    ///
    /// @return `true` if degenerated, `false` otherwise
    [[nodiscard]] bool is_degenerated() const;

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return Location in 3D space corresponding to the parametrical position
    [[nodiscard]] Point point(double u) const;

    /// Compute first derivative at parametrical position
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return First derivative
    [[nodiscard]] Vector d1(double u) const;

    /// Get curvature
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return Curvature at location `u`
    [[nodiscard]] double curvature(double u) const;

    /// Get length of the curve
    ///
    /// @return Length of the curve
    [[nodiscard]] double length() const;

    /// Get range of the parameter
    ///
    /// @return Range as a tuple [lower, upper]
    [[nodiscard]] std::tuple<double, double> param_range() const;

    /// Get the first edge vertex
    ///
    /// @return First edge vertex
    [[nodiscard]] GeomVertex first_vertex() const;

    /// Get the last edge vertex
    ///
    /// @return Last edge vertex
    [[nodiscard]] GeomVertex last_vertex() const;

    /// Get parameter on the curve from a physical location
    ///
    /// @param pt Physical location
    /// @return Parameter
    [[nodiscard]] double parameter_from_point(const Point & pt) const;

    /// Find nearest point
    ///
    /// @param pt Physical point
    /// @return Point on the curve, nearest to `pt`
    [[nodiscard]] Point nearest_point(const Point & pt) const;

    /// Check if point is on the curve
    ///
    /// @param pt Point to investigate
    /// @return `true` if the point is on the curve, `false` otherwise
    [[nodiscard]] bool contains_point(const Point & pt) const;

    bool is_seam(const GeomSurface & surface) const;

    operator const TopoDS_Shape &() const;

    operator const TopoDS_Edge &() const;

private:
    TopoDS_Edge edge_;
    Handle(Geom_Curve) curve_;
    /// Curve type
    CurveType crv_type_;
    /// Minimum value of the parameter
    double umin_;
    /// Maximum value of the parameter
    double umax_;
    /// Curve length
    double len_;
};

} // namespace krado
