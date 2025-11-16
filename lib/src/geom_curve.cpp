// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_CLProps.hxx"
#include "BRepAdaptor_Curve.hxx"
#include "GProp_GProps.hxx"
#include "TopExp.hxx"
#include "Geom_BSplineCurve.hxx"
#include "Geom_BezierCurve.hxx"
#include "Geom_Line.hxx"
#include "Geom_Circle.hxx"

namespace krado {

GeomCurve::GeomCurve(const TopoDS_Edge & edge) : GeomShape(edge), edge_(edge), umin_(0), umax_(0)
{
    // force orientation of internal/external edges, otherwise reverse will not produce the expected
    // result
    if (this->edge_.Orientation() == TopAbs_INTERNAL ||
        this->edge_.Orientation() == TopAbs_EXTERNAL) {
        this->edge_ = TopoDS::Edge(this->edge_.Oriented(TopAbs_FORWARD));
    }
    this->curve_ = BRep_Tool::Curve(this->edge_, this->umin_, this->umax_);
    if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_BSplineCurve))
        this->crv_type_ = BSpline;
    else if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_BezierCurve))
        this->crv_type_ = Bezier;
    else if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_Line))
        this->crv_type_ = Line;
    else if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_Circle))
        this->crv_type_ = Circle;
    else
        this->crv_type_ = Unknown;

    GProp_GProps props;
    BRepGProp::LinearProperties(this->edge_, props);
    this->len_ = props.Mass();
}

GeomCurve::CurveType
GeomCurve::type() const
{
    return this->crv_type_;
}

bool
GeomCurve::is_degenerated() const
{
    return BRep_Tool::Degenerated(this->edge_);
}

Point
GeomCurve::point(double u) const
{
    gp_Pnt pnt = this->curve_->Value(u);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Vector
GeomCurve::d1(double u) const
{
    BRepAdaptor_Curve brepc(this->edge_);
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
    BRepAdaptor_Curve brepc(this->edge_);
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
    return this->len_;
}

std::tuple<double, double>
GeomCurve::param_range() const
{
    return { this->umin_, this->umax_ };
}

GeomVertex
GeomCurve::first_vertex() const
{
    return GeomVertex(TopExp::FirstVertex(this->edge_));
}

GeomVertex
GeomCurve::last_vertex() const
{
    return GeomVertex(TopExp::LastVertex(this->edge_));
}

double
GeomCurve::parameter_from_point(const Point & pt) const
{
    GeomAPI_ProjectPointOnCurve proj_pt_on_curve;
    proj_pt_on_curve.Init(this->curve_, this->umin_, this->umax_);
    gp_Pnt pnt(pt.x, pt.y, pt.z);
    proj_pt_on_curve.Perform(pnt);
    if (proj_pt_on_curve.NbPoints() > 0) {
        auto u = proj_pt_on_curve.LowerDistanceParameter();
        return u;
    }
    else
        throw Exception("Projection of point failed to find parameter");
}

Point
GeomCurve::nearest_point(const Point & pt) const
{
    GeomAPI_ProjectPointOnCurve proj_pt_on_curve;
    proj_pt_on_curve.Init(this->curve_, this->umin_, this->umax_);
    gp_Pnt gpnt(pt.x, pt.y, pt.z);
    proj_pt_on_curve.Perform(gpnt);
    if (proj_pt_on_curve.NbPoints()) {
        gpnt = proj_pt_on_curve.NearestPoint();
        return Point(gpnt.X(), gpnt.Y(), gpnt.Z());
    }
    else
        throw Exception("Projection of point failed to find parameter");
}

bool
GeomCurve::contains_point(const Point & pt) const
{
    Point xyz = nearest_point(pt);
    const Standard_Real tolerance = BRep_Tool::Tolerance(this->edge_);
    if (pt.distance(xyz) <= tolerance)
        return true;
    else
        return false;
}

bool
GeomCurve::is_seam(const GeomSurface & surface) const
{
    TopLoc_Location l;
    const Handle(Geom_Surface) & surf = BRep_Tool::Surface(surface, l);
    return BRep_Tool::IsClosed(this->edge_, surf, l);
}

GeomCurve::operator const TopoDS_Shape &() const
{
    return this->edge_;
}

GeomCurve::operator const TopoDS_Edge &() const
{
    return this->edge_;
}

const Handle(Geom_Curve) & GeomCurve::curve_handle() const
{
    return this->curve_;
}

// - - -

Point
get_circle_center(const GeomCurve & crv)
{
    if (crv.type() != GeomCurve::Circle)
        throw Exception("Curve {} is not a circle", crv.id());

    const Handle(Geom_Circle) & circle = Handle(Geom_Circle)::DownCast(crv.curve_);
    gp_Pnt center = circle->Location();
    return Point(center.X(), center.Y(), center.Z());
}

} // namespace krado
