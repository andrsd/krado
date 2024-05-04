#pragma once

#include "TopoDS_Edge.hxx"
#include "Geom_Curve.hxx"

namespace krado::geo {

class Curve {
public:
    Curve(const TopoDS_Edge & edge);

private:
    TopoDS_Edge edge;
    Handle(Geom_Curve) curve;
    double first, last;
};

} // namespace krado::geo
