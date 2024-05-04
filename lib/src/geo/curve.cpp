#include "krado/geo/curve.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"

namespace krado::geo {

Curve::Curve(const TopoDS_Edge & edge) : edge(edge)
{
    // force orientation of internal/external edges, otherwise reverse will not produce the expected
    // result
    if (this->edge.Orientation() == TopAbs_INTERNAL ||
        this->edge.Orientation() == TopAbs_EXTERNAL) {
        this->edge = TopoDS::Edge(this->edge.Oriented(TopAbs_FORWARD));
    }
    this->curve = BRep_Tool::Curve(this->edge, this->first, this->last);
}

} // namespace krado::geo
