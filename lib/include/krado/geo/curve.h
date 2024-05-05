#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include "krado/geo/vertex.h"
#include "TopoDS_Edge.hxx"
#include "Geom_Curve.hxx"

namespace krado::geo {

class Curve {
public:
    explicit Curve(const TopoDS_Edge & edge);

    /// Check if the edge is degenerated
    ///
    /// @return `true` if degenerated, `false` otherwise
    bool is_degenerated() const;

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return Location in 3D space corresponding to the parametrical position
    Point point(double u) const;

    /// Compute first derivative at parameterical position
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
    Vertex first_vertex() const;

    /// Get the last edge vertex
    ///
    /// @return Last edge vertex
    Vertex last_vertex() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Edge edge;
    Handle(Geom_Curve) curve;
    double first, last;
    /// Curve length
    double len;
};

} // namespace krado::geo
