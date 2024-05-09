#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include "krado/geom_vertex.h"
#include "TopoDS_Edge.hxx"
#include "Geom_Curve.hxx"
#include "GeomAPI_ProjectPointOnCurve.hxx"

namespace krado {

class Model;

class GeomCurve {
public:
    explicit GeomCurve(const TopoDS_Edge & edge);

    /// Check if the edge is degenerated
    ///
    /// @return `true` if degenerated, `false` otherwise
    bool is_degenerated() const;

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return Location in 3D space corresponding to the parametrical position
    Point point(double u) const;

    /// Compute first derivative at parametrical position
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return First derivative
    Vector d1(double u) const;

    /// Get curvature
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return Curvature at location `u`
    double curvature(double u) const;

    /// Get length of the curve
    ///
    /// @return Length of the curve
    double length() const;

    /// Get range of the parameter
    ///
    /// @return Range as a tuple [lower, upper]
    std::tuple<double, double> param_range() const;

    /// Get the first edge vertex
    ///
    /// @return First edge vertex
    GeomVertex first_vertex() const;

    /// Get the last edge vertex
    ///
    /// @return Last edge vertex
    GeomVertex last_vertex() const;

    /// Get parameter on the curve from a physical location
    ///
    /// @param pt Physical location
    /// @return Parameter
    double parameter_from_point(const Point & pt) const;

    /// Find nearest point
    ///
    /// @param pt Physical point
    /// @return Point on the curve, nearest to `pt`
    Point nearest_point(const Point & pt) const;

    /// Check if point is on the curve
    ///
    /// @param pt Point to investigate
    /// @return `true` if the point is on the curve, `false` otherwise
    bool contains_point(const Point & pt) const;

    operator const TopoDS_Shape &() const;

private:
    std::tuple<bool, double> project(const Point & pt) const;

    TopoDS_Edge edge;
    Handle(Geom_Curve) curve;
    /// Minimum value of the parameter
    double umin;
    /// Maximum value of the parameter
    double umax;
    /// Curve length
    double len;
    ///
    mutable GeomAPI_ProjectPointOnCurve proj_pt_on_curve;
};

} // namespace krado
