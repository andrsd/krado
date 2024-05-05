#include "krado/geom_surface.h"
#include "TopoDS.hxx"
#include "BRep_Tool.hxx"
#include "BRepGProp.hxx"
#include "BRepLProp_SLProps.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "GProp_GProps.hxx"
#include "TopExp.hxx"

namespace krado {

GeomSurface::GeomSurface(const TopoDS_Face & face) : face(face)
{
    this->surface = BRep_Tool::Surface(this->face);

    GProp_GProps props;
    BRepGProp::SurfaceProperties(this->face, props);
    this->surf_area = props.Mass();
}

Point
GeomSurface::point(double u, double v) const
{
    gp_Pnt pnt = this->surface->Value(u, v);
    return Point(pnt.X(), pnt.Y(), pnt.Z());
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

GeomSurface::operator const TopoDS_Shape &() const
{
    return this->face;
}

} // namespace krado
