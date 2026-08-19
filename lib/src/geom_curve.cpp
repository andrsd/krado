// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_model.h"
#include "krado/exception.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/consts.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepLProp_CLProps.hxx"
#include "BRepAdaptor_Curve.hxx"
#include "TopExp.hxx"
#include "Geom_BSplineCurve.hxx"
#include "Geom_BezierCurve.hxx"
#include "Geom_Line.hxx"
#include "Geom_Circle.hxx"

namespace krado {
namespace {

// force orientation of internal/external edges, otherwise reverse will not produce the expected
// result
const TopoDS_Edge &
get_oriented_edge(const TopoDS_Edge & edge)
{
    if (edge.Orientation() == TopAbs_INTERNAL || edge.Orientation() == TopAbs_EXTERNAL)
        return TopoDS::Edge(edge.Oriented(TopAbs_FORWARD));
    else
        return edge;
}

} // namespace

GeomCurve::GeomCurve(const TopoDS_Edge & edge) :
    GeomShape(get_oriented_edge(edge)),
    umin_(0),
    umax_(0)
{
    const auto & edge1 = TopoDS::Edge(this->shape_);
    this->curve_ = BRep_Tool::Curve(edge1, this->umin_, this->umax_);
    if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_BSplineCurve))
        this->crv_type_ = CurveType::BSpline;
    else if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_BezierCurve))
        this->crv_type_ = CurveType::Bezier;
    else if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_Line))
        this->crv_type_ = CurveType::Line;
    else if (this->curve_->DynamicType() == STANDARD_TYPE(Geom_Circle))
        this->crv_type_ = CurveType::Circle;
    else
        this->crv_type_ = CurveType::Unknown;
}

int
GeomCurve::dim() const
{
    return 1;
}

GeomCurve::CurveType
GeomCurve::type() const
{
    return this->crv_type_;
}

GeomCurve::Orientation
GeomCurve::orientation() const
{
    const auto & edge = TopoDS::Edge(this->shape_);
    if (edge.Orientation() == TopAbs_FORWARD)
        return Orientation::Forward;
    else
        return Orientation::Reversed;
}

bool
GeomCurve::is_degenerated() const
{
    const auto & edge = TopoDS::Edge(this->shape_);
    return BRep_Tool::Degenerated(edge);
}

Point
GeomCurve::point(double u) const
{
    return Point::create(this->curve_->Value(u));
}

Vector
GeomCurve::d1(double u) const
{
    const auto & edge = TopoDS::Edge(this->shape_);
    BRepAdaptor_Curve brepc(edge);
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

    const auto & edge = TopoDS::Edge(this->shape_);
    BRepAdaptor_Curve brepc(edge);
    BRepLProp_CLProps prop(brepc, 2, 1e-15);
    prop.SetParameter(u);
    if (!prop.IsTangentDefined())
        return 0.;
    else
        return prop.Curvature();
}

std::tuple<double, double>
GeomCurve::param_range() const
{
    return { this->umin_, this->umax_ };
}

GeomVertex
GeomCurve::first_vertex() const
{
    const auto & edge = TopoDS::Edge(this->shape_);
    return GeomVertex(TopExp::FirstVertex(edge));
}

GeomVertex
GeomCurve::last_vertex() const
{
    const auto & edge = TopoDS::Edge(this->shape_);
    return GeomVertex(TopExp::LastVertex(edge));
}

double
GeomCurve::parameter_from_point(Point pt) const
{
    GeomAPI_ProjectPointOnCurve proj_pt_on_curve;
    proj_pt_on_curve.Init(this->curve_, this->umin_, this->umax_);
    proj_pt_on_curve.Perform(pt);
    if (proj_pt_on_curve.NbPoints() > 0) {
        auto u = proj_pt_on_curve.LowerDistanceParameter();
        return u;
    }
    else
        throw Exception("Projection of point failed to find parameter");
}

Point
GeomCurve::nearest_point(Point pt) const
{
    GeomAPI_ProjectPointOnCurve proj_pt_on_curve;
    proj_pt_on_curve.Init(this->curve_, this->umin_, this->umax_);
    proj_pt_on_curve.Perform(pt);
    if (proj_pt_on_curve.NbPoints() > 0)
        return Point::create(proj_pt_on_curve.NearestPoint());
    else
        throw Exception("Projection of point failed to find parameter");
}

bool
GeomCurve::contains_point(Point pt) const
{
    Point xyz = nearest_point(pt);
    const auto & edge = TopoDS::Edge(this->shape_);
    const Standard_Real tolerance = BRep_Tool::Tolerance(edge);
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
    const auto & edge = TopoDS::Edge(this->shape_);
    return BRep_Tool::IsClosed(edge, surf, l);
}

double
GeomCurve::mesh_size_at_param(double u) const
{
    auto fv = first_vertex();
    auto lv = last_vertex();

    auto [u_lo, u_hi] = param_range();
    if (this->mesh_size_.has_value()) {
        return this->mesh_size_.value();
    }
    else if (not fv.is_null() && not lv.is_null()) {
        // 2 bounding vertices => interpolate the size
        auto lc1 = fv.mesh_size();
        auto lc2 = lv.mesh_size();
        auto alpha = (u - u_lo) / (u_hi - u_lo);
        return ((1 - alpha) * lc1) + (alpha * lc2);
    }
    else if (not fv.is_null() && std::abs(u - u_lo) < EPSILON) {
        return fv.mesh_size();
    }
    else if (not lv.is_null() && std::abs(u - u_hi) < EPSILON) {
        return lv.mesh_size();
    }
    else
        return MAX_LC;
}

GeomCurve::operator const TopoDS_Edge &() const
{
    return TopoDS::Edge(this->shape_);
}

const Handle(Geom_Curve) & GeomCurve::curve_handle() const
{
    return this->curve_;
}

// - - -

Point
get_circle_center(const GeomCurve & crv)
{
    if (crv.type() != GeomCurve::CurveType::Circle)
        throw Exception("Curve is not a circle");

    const Handle(Geom_Circle) & circle = Handle(Geom_Circle)::DownCast(crv.curve_);
    return Point::create(circle->Location());
}

double
get_circle_radius(const GeomCurve & crv)
{
    if (crv.type() != GeomCurve::CurveType::Circle)
        throw Exception("Curve is not a circle");

    const Handle(Geom_Circle) & circle = Handle(Geom_Circle)::DownCast(crv.curve_);
    return circle->Radius();
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::GeomCurve::CurveType & type)
{
    switch (type) {
    case krado::GeomCurve::CurveType::Line:
        stream << "line";
        break;

    case krado::GeomCurve::CurveType::Circle:
        stream << "circle";
        break;

    case krado::GeomCurve::CurveType::BSpline:
        stream << "bspline";
        break;

    case krado::GeomCurve::CurveType::Bezier:
        stream << "bezier";
        break;

    case krado::GeomCurve::CurveType::Unknown:
    default:
        stream << "unknown";
        break;
    }
    return stream;
}

std::ostream &
operator<<(std::ostream & stream, const krado::GeomCurve::Orientation & ori)
{
    switch (ori) {
    case krado::GeomCurve::Orientation::Forward:
        stream << "forward";
        break;
    case krado::GeomCurve::Orientation::Reversed:
        stream << "reversed";
        break;
    default:
        stream << "unknown";
        break;
    }
    return stream;
}

std::ostream &
operator<<(std::ostream & stream, const krado::GeomCurve & crv)
{
    stream << "Curve: ";
    stream << "type=" << crv.type() << ", ";
    auto [umin, umax] = crv.param_range();
    stream << "u=[" << umin << ", " << umax << "], ";
    stream << "length=" << crv.length();
    return stream;
}
