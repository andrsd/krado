#include "krado/geo/curve.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_CLProps.hxx"
#include "BRepAdaptor_Curve.hxx"
#include "GProp_GProps.hxx"
#include "TopExp.hxx"

namespace krado::geo {

Curve::Curve(const TopoDS_Edge & edge) :
    edge(edge),
    first(0),
    last(0)
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

bool
Curve::is_degenerated() const
{
    return BRep_Tool::Degenerated(this->edge);
}

Point
Curve::point(double u) const
{
    gp_Pnt pnt = this->curve->Value(u);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Vector
Curve::d1(double u) const
{
    BRepAdaptor_Curve brepc(this->edge);
    BRepLProp_CLProps prop(brepc, 1, 1e-10);
    prop.SetParameter(u);
    gp_Vec d1 = prop.D1();
    return Vector(d1.X(), d1.Y(), d1.Z());
}

double
Curve::curvature(double u) const
{
    if (is_degenerated())
        return 0.;

    Standard_Real curvature;
    BRepAdaptor_Curve brepc(this->edge);
    BRepLProp_CLProps prop(brepc, 2, 1e-15);
    prop.SetParameter(u);
    if (!prop.IsTangentDefined())
        curvature = 0.;
    else
        curvature = prop.Curvature();
    return curvature;
}

double
Curve::length() const
{
    return this->len;
}

std::tuple<double, double>
Curve::param_range() const
{
    return { this->first, this->last };
}

Vertex
Curve::first_vertex() const
{
    return Vertex(TopExp::FirstVertex(this->edge));
}

Vertex
Curve::last_vertex() const
{
    return Vertex(TopExp::LastVertex(this->edge));
}

Curve::operator const TopoDS_Shape &() const
{
    return this->edge;
}

} // namespace krado::geo
