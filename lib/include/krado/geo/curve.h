#pragma once

#include "krado/point.h"
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

    /// Get length of the curve
    ///
    /// @return Length of the curve
    double length() const;

    /// Get range of the parameter
    ///
    /// @return Range as a tuple [lower, upper]
    std::tuple<double, double> param_range() const;

private:
    TopoDS_Edge edge;
    Handle(Geom_Curve) curve;
    double first, last;
    /// Curve length
    double len;
};

} // namespace krado::geo
