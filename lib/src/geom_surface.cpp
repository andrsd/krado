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

GeomSurface::operator const TopoDS_Shape &() const
{
    return this->face;
}

} // namespace krado
