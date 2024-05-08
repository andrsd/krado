#include "krado/geom_curve.h"
#include "krado/exception.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_CLProps.hxx"
#include "BRepAdaptor_Curve.hxx"
#include "GProp_GProps.hxx"
#include "TopExp.hxx"

namespace krado {

GeomCurve::GeomCurve(const TopoDS_Edge & edge) : edge(edge), umin(0), umax(0)
{
    // force orientation of internal/external edges, otherwise reverse will not produce the expected
    // result
    if (this->edge.Orientation() == TopAbs_INTERNAL ||
        this->edge.Orientation() == TopAbs_EXTERNAL) {
        this->edge = TopoDS::Edge(this->edge.Oriented(TopAbs_FORWARD));
    }
    this->curve = BRep_Tool::Curve(this->edge, this->umin, this->umax);

    GProp_GProps props;
    BRepGProp::LinearProperties(this->edge, props);
    this->len = props.Mass();

    this->proj_pt_on_curve.Init(this->curve, this->umin, this->umax);
}

bool
GeomCurve::is_degenerated() const
{
    return BRep_Tool::Degenerated(this->edge);
}

Point
GeomCurve::point(double u) const
{
    gp_Pnt pnt = this->curve->Value(u);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Vector
GeomCurve::d1(double u) const
{
    BRepAdaptor_Curve brepc(this->edge);
    BRepLProp_CLProps prop(brepc, 1, 1e-10);
    prop.SetParameter(u);
    gp_Vec d1 = prop.D1();
    return Vector(d1.X(), d1.Y(), d1.Z());
}

double
GeomCurve::curvature(double u) const
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
GeomCurve::length() const
{
    return this->len;
}

std::tuple<double, double>
GeomCurve::param_range() const
{
    return { this->umin, this->umax };
}

GeomVertex
GeomCurve::first_vertex() const
{
    return GeomVertex(TopExp::FirstVertex(this->edge));
}

GeomVertex
GeomCurve::last_vertex() const
{
    return GeomVertex(TopExp::LastVertex(this->edge));
}

double
GeomCurve::parameter_from_point(const Point & pt) const
{
    auto [found, par] = project(pt);
    if (found)
        return par;
    else
        throw Exception("Projection of point failed to find parameter");
}

Point
GeomCurve::nearest_point(const Point & pt) const
{
    gp_Pnt gpnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_curve.Perform(gpnt);
    if (this->proj_pt_on_curve.NbPoints()) {
        gpnt = this->proj_pt_on_curve.NearestPoint();
        return Point(gpnt.X(), gpnt.Y(), gpnt.Z());
    }
    else
        throw Exception("Projection of point failed to find parameter");
}

bool
GeomCurve::contains_point(const Point & pt) const
{
    Point xyz = nearest_point(pt);
    const Standard_Real tolerance = BRep_Tool::Tolerance(this->edge);
    if (pt.distance(xyz) <= tolerance)
        return true;
    else
        return false;
}

GeomCurve::operator const TopoDS_Shape &() const
{
    return this->edge;
}

std::tuple<bool, double>
GeomCurve::project(const Point & pt) const
{
    gp_Pnt pnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_curve.Perform(pnt);

    if (this->proj_pt_on_curve.NbPoints() > 0) {
        auto u = this->proj_pt_on_curve.LowerDistanceParameter();
        return { true, u };
    }
    else
        return { false, 0. };
}

} // namespace krado
