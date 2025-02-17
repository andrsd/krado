// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_SLProps.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "GProp_GProps.hxx"
#include "TopExp_Explorer.hxx"
#include "ShapeAnalysis.hxx"
#include "BRepClass_FaceClassifier.hxx"
#include "krado/geom_shape.h"

namespace krado {

GeomSurface::GeomSurface(const TopoDS_Face & face) : GeomShape(face), face_(face)
{
    this->surface_ = BRep_Tool::Surface(this->face_);

    GProp_GProps props;
    BRepGProp::SurfaceProperties(this->face_, props);
    this->surf_area_ = props.Mass();

    ShapeAnalysis::GetFaceUVBounds(this->face_, this->umin_, this->umax_, this->vmin_, this->vmax_);

    this->proj_pt_on_surface_.Init(this->surface_,
                                   this->umin_,
                                   this->umax_,
                                   this->vmin_,
                                   this->vmax_);
}

GeomSurface::GeomSurface(const GeomSurface & other) :
    GeomShape(other),
    face_(other.face_),
    surface_(other.surface_),
    surf_area_(other.surf_area_),
    umin_(other.umin_),
    umax_(other.umax_),
    vmin_(other.vmin_),
    vmax_(other.vmax_)
{
    this->proj_pt_on_surface_.Init(this->surface_,
                                   this->umin_,
                                   this->umax_,
                                   this->vmin_,
                                   this->vmax_);
}

GeomSurface::GeomSurface(GeomSurface && other) :
    GeomShape(other),
    face_(other.face_),
    surface_(other.surface_),
    surf_area_(other.surf_area_),
    umin_(other.umin_),
    umax_(other.umax_),
    vmin_(other.vmin_),
    vmax_(other.vmax_)
{
    this->proj_pt_on_surface_.Init(this->surface_,
                                   this->umin_,
                                   this->umax_,
                                   this->vmin_,
                                   this->vmax_);
}

Point
GeomSurface::point(double u, double v) const
{
    gp_Pnt pnt = this->surface_->Value(u, v);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Vector
GeomSurface::normal(double u, double v) const
{
    BRepAdaptor_Surface breps(this->face_);
    BRepLProp_SLProps prop(breps, 1, 1e-10);
    prop.SetParameters(u, v);
    auto n = prop.Normal();
    return Vector(n.X(), n.Y(), n.Z());
}

std::tuple<Vector, Vector>
GeomSurface::d1(double u, double v) const
{
    BRepAdaptor_Surface breps(this->face_);
    BRepLProp_SLProps prop(breps, 1, 1e-10);
    prop.SetParameters(u, v);
    auto d1u = prop.D1U();
    auto d1v = prop.D1V();
    return { Vector(d1u.X(), d1u.Y(), d1u.Z()), Vector(d1v.X(), d1v.Y(), d1v.Z()) };
}

double
GeomSurface::area() const
{
    return this->surf_area_;
}

std::tuple<double, double>
GeomSurface::param_range(int i) const
{
    if (i == 0)
        return { this->umin_, this->umax_ };
    else if (i == 1)
        return { this->vmin_, this->vmax_ };
    else
        throw Exception("Incorrect index.");
}

std::vector<GeomCurve>
GeomSurface::curves() const
{
    std::vector<GeomCurve> crvs;
    TopExp_Explorer exp;
    for (exp.Init(this->face_, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());
        auto gcurve = GeomCurve(edge);
        crvs.emplace_back(gcurve);
    }
    return crvs;
}

std::tuple<double, double>
GeomSurface::parameter_from_point(const Point & pt) const
{
    auto [found, u, v] = project(pt);
    if (found)
        return { u, v };
    else
        throw Exception("Projection of point failed to find parameter");
}

GeomSurface::operator const TopoDS_Shape &() const
{
    return this->face_;
}

std::tuple<bool, double, double>
GeomSurface::project(const Point & pt) const
{
    gp_Pnt pnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_surface_.Perform(pnt);

    if (this->proj_pt_on_surface_.NbPoints() > 0) {
        double u, v;
        this->proj_pt_on_surface_.LowerDistanceParameters(u, v);
        return { true, u, v };
    }
    else
        return { false, 0., 0. };
}

Point
GeomSurface::nearest_point(const Point & pt) const
{
    gp_Pnt gpnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_surface_.Perform(gpnt);
    if (this->proj_pt_on_surface_.NbPoints()) {
        gpnt = this->proj_pt_on_surface_.NearestPoint();
        return Point(gpnt.X(), gpnt.Y(), gpnt.Z());
    }
    else
        throw Exception("Projection of point failed to find parameter");
}

bool
GeomSurface::contains_point(const Point & pt) const
{
    Point xyz = nearest_point(pt);
    const Standard_Real tolerance = BRep_Tool::Tolerance(this->face_);
    if (pt.distance(xyz) <= tolerance)
        return true;
    else
        return false;
}

} // namespace krado
