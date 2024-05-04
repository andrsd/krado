#pragma once

#include "krado/point.h"
#include "TopoDS_Edge.hxx"
#include "Geom_Curve.hxx"

namespace krado::geo {

class Curve {
public:
    Curve(const TopoDS_Edge & edge);

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location [0..<length_of_curve>]
    /// @return Location in 3D space corresponding to the parameterical position
    Point point(double u) const;

private:
    TopoDS_Edge edge;
    Handle(Geom_Curve) curve;
    double first, last;
};

} // namespace krado::geo
