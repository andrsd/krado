// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/predicates.h"
#include "krado/geom_shape.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_SLProps.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "GProp_GProps.hxx"
#include "TopExp_Explorer.hxx"
#include "ShapeAnalysis.hxx"
#include "BRepClass_FaceClassifier.hxx"

namespace krado {

GeomSurface::GeomSurface(const TopoDS_Face & face) : GeomShape(2, face), face_(face)
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
    gp_Pnt pnt = this->surface_->Value(param.u, param.v);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
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
    gp_Pnt pnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_surface_.Perform(pnt);

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
    auto xyz = nearest_point(pt);
    const auto tolerance = BRep_Tool::Tolerance(this->face_);
    if (pt.distance(xyz) <= tolerance)
        return true;
    else
        return false;
}

const std::set<GeomVertex *> &
GeomSurface::embedded_vertices() const
{
    return this->embedded_vtxs_;
}

const std::vector<GeomCurve *> &
GeomSurface::embedded_curves() const
{
    return this->embedded_crvs_;
}

bool
point_inside_parametric_domain(std::vector<UVParam> & bnd, UVParam & p, UVParam & out, int & N)
{
    int count = 0;
    for (size_t i = 0; i < bnd.size(); i += 2) {
        UVParam p1 = bnd[i];
        UVParam p2 = bnd[i + 1];
        double a = orient2d(p1, p2, p);
        double b = orient2d(p1, p2, out);
        if (a * b < 0) {
            a = orient2d(p, out, p1);
            b = orient2d(p, out, p2);
            if (a * b < 0)
                count++;
        }
    }
    N = count;
    if (count % 2 == 0)
        return false;
    return true;
}

} // namespace krado
