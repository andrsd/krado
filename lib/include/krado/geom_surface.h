#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include "krado/geom_curve.h"
#include "TopoDS_Face.hxx"
#include "Geom_Surface.hxx"
#include "GeomAPI_ProjectPointOnSurf.hxx"
#include <vector>

namespace krado {

class Model;

class GeomSurface {
public:
    explicit GeomSurface(const TopoDS_Face & face);
    GeomSurface(const GeomSurface & other);
    GeomSurface(GeomSurface && other);

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Location in 3D space corresponding to the parametrical position
    Point point(double u, double v) const;

    /// Get normal vector at parametrical location
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Normal vector at location (u, v)
    Vector normal(double u, double v) const;

    /// Compute first derivative at parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return First derivative
    std::tuple<Vector, Vector> d1(double u, double v) const;

    /// Get area of the surface
    ///
    /// @return Area of the surface
    double area() const;

    /// Get range of the parameter
    ///
    /// @return Range as a tuple [lower, upper]
    std::tuple<double, double> param_range(int i) const;

    /// Get curves bounding this surface
    ///
    /// @return Curves bounding the surface
    std::vector<GeomCurve> curves() const;

    /// Get parameter on the surface from a physical location
    ///
    /// @param pt Physical location
    /// @return Parameters (u, v)
    std::tuple<double, double> parameter_from_point(const Point &pt) const;

    operator const TopoDS_Shape &() const;

private:
    std::tuple<bool, double, double> project(const Point &pt) const;

    TopoDS_Face face;
    Handle(Geom_Surface) surface;
    /// Surface area
    double surf_area;
    double umin, umax;
    double vmin, vmax;
    /// Needs to be a pointer, because GeomSurface must be movable
    mutable GeomAPI_ProjectPointOnSurf proj_pt_on_surface;
};

} // namespace krado
