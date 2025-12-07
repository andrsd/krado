// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/geom_shape.h"
#include "krado/occ.h"
#include "krado/wire.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_SLProps.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "GProp_GProps.hxx"
#include "TopExp_Explorer.hxx"
#include "ShapeAnalysis.hxx"
#include "BRepClass_FaceClassifier.hxx"
#include "Geom_Circle.hxx"

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
GeomSurface::point(const UVParam & param) const
{
    return Point::create(this->surface_->Value(param.u, param.v));
}

Vector
GeomSurface::normal(const UVParam & param) const
{
    BRepAdaptor_Surface breps(this->face_);
    BRepLProp_SLProps prop(breps, 1, 1e-10);
    prop.SetParameters(param.u, param.v);
    auto n = prop.Normal();
    return Vector(n.X(), n.Y(), n.Z());
}

std::tuple<Vector, Vector>
GeomSurface::d1(const UVParam & param) const
{
    BRepAdaptor_Surface breps(this->face_);
    BRepLProp_SLProps prop(breps, 1, 1e-10);
    prop.SetParameters(param.u, param.v);
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

UVParam
GeomSurface::parameter_from_point(const Point & pt) const
{
    auto [found, uv] = project(pt);
    if (found)
        return uv;
    else
        throw Exception("Projection of point failed to find parameter");
}

GeomSurface::operator const TopoDS_Shape &() const
{
    return this->face_;
}

GeomSurface::operator const TopoDS_Face &() const
{
    return this->face_;
}

std::tuple<bool, UVParam>
GeomSurface::project(const Point & pt) const
{
    this->proj_pt_on_surface_.Perform(pt);

    if (this->proj_pt_on_surface_.NbPoints() > 0) {
        double u, v;
        this->proj_pt_on_surface_.LowerDistanceParameters(u, v);
        return { true, { u, v } };
    }
    else
        return { false, { 0., 0. } };
}

Point
GeomSurface::nearest_point(const Point & pt) const
{
    this->proj_pt_on_surface_.Perform(pt);
    if (this->proj_pt_on_surface_.NbPoints())
        return Point::create(this->proj_pt_on_surface_.NearestPoint());
    else
        throw Exception("Projection of point failed to find parameter");
}

bool
GeomSurface::contains_point(const Point & pt) const
{
    auto xyz = nearest_point(pt);
    const auto tolerance = BRep_Tool::Tolerance(this->face_);
    if (pt.distance(xyz) <= tolerance)
        return true;
    else
        return false;
}

GeomSurface
GeomSurface::create(const Wire & wire)
{
    BRepBuilderAPI_MakeFace make_face(wire);
    make_face.Build();
    if (!make_face.IsDone())
        throw Exception("Face was not created");
    return GeomSurface(make_face.Face());
}

// - - -

bool
is_circular_face(const GeomSurface & surface)
{
    TopExp_Explorer exp(surface, TopAbs_EDGE);
    if (!exp.More())
        return false;
    auto edge = TopoDS::Edge(exp.Current());
    Standard_Real f, l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, f, l);
    return !curve.IsNull() && curve->IsKind(STANDARD_TYPE(Geom_Circle));
}

} // namespace krado
