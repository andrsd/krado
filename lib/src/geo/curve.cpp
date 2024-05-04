#include "krado/geo/curve.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"

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

    GProp_GProps props;
    BRepGProp::LinearProperties(this->edge, props);
    this->len = props.Mass();
}

Point
Curve::point(double u) const
{
    gp_Pnt pnt = this->curve->Value(u);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

double
Curve::length() const
{
    return this->len;
}

} // namespace krado::geo
