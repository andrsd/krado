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

namespace krado {

GeomSurface::GeomSurface(const TopoDS_Face & face) : face(face)
{
    this->surface = BRep_Tool::Surface(this->face);

    GProp_GProps props;
    BRepGProp::SurfaceProperties(this->face, props);
    this->surf_area = props.Mass();

    ShapeAnalysis::GetFaceUVBounds(this->face, this->umin, this->umax, this->vmin, this->vmax);

    this->proj_pt_on_surface.Init(this->surface, this->umin, this->umax, this->vmin, this->vmax);
}

GeomSurface::GeomSurface(const GeomSurface & other) :
    face(other.face),
    surface(other.surface),
    surf_area(other.surf_area),
    umin(other.umin),
    umax(other.umax),
    vmin(other.vmin),
    vmax(other.vmax)
{
    this->proj_pt_on_surface.Init(this->surface, this->umin, this->umax, this->vmin, this->vmax);
}

GeomSurface::GeomSurface(GeomSurface && other) :
    face(other.face),
    surface(other.surface),
    surf_area(other.surf_area),
    umin(other.umin),
    umax(other.umax),
    vmin(other.vmin),
    vmax(other.vmax)
{
    this->proj_pt_on_surface.Init(this->surface, this->umin, this->umax, this->vmin, this->vmax);
}

Point
GeomSurface::point(double u, double v) const
{
    gp_Pnt pnt = this->surface->Value(u, v);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Vector
GeomSurface::normal(double u, double v) const
{
    BRepAdaptor_Surface breps(this->face);
    BRepLProp_SLProps prop(breps, 1, 1e-10);
    prop.SetParameters(u, v);
    auto n = prop.Normal();
    return Vector(n.X(), n.Y(), n.Z());
}

std::tuple<Vector, Vector>
GeomSurface::d1(double u, double v) const
{
    BRepAdaptor_Surface breps(this->face);
    BRepLProp_SLProps prop(breps, 1, 1e-10);
    prop.SetParameters(u, v);
    auto d1u = prop.D1U();
    auto d1v = prop.D1V();
    return { Vector(d1u.X(), d1u.Y(), d1u.Z()), Vector(d1v.X(), d1v.Y(), d1v.Z()) };
}

double
GeomSurface::area() const
{
    return this->surf_area;
}

std::tuple<double, double>
GeomSurface::param_range(int i) const
{
    if (i == 0)
        return { this->umin, this->umax };
    else if (i == 1)
        return { this->vmin, this->vmax };
    else
        throw Exception("Incorrect index.");
}

std::vector<GeomCurve>
GeomSurface::curves() const
{
    std::vector<GeomCurve> crvs;
    TopExp_Explorer exp;
    for (exp.Init(this->face, TopAbs_EDGE); exp.More(); exp.Next()) {
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
    return this->face;
}

std::tuple<bool, double, double>
GeomSurface::project(const Point & pt) const
{
    gp_Pnt pnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_surface.Perform(pnt);

    if (this->proj_pt_on_surface.NbPoints() > 0) {
        double u, v;
        this->proj_pt_on_surface.LowerDistanceParameters(u, v);
        return { true, u, v };
    }
    else
        return { false, 0., 0. };
}

Point
GeomSurface::nearest_point(const Point & pt) const
{
    gp_Pnt gpnt(pt.x, pt.y, pt.z);
    this->proj_pt_on_surface.Perform(gpnt);
    if (this->proj_pt_on_surface.NbPoints()) {
        gpnt = this->proj_pt_on_surface.NearestPoint();
        return Point(gpnt.X(), gpnt.Y(), gpnt.Z());
    }
    else
        throw Exception("Projection of point failed to find parameter");
}

} // namespace krado
