#pragma once

#include "krado/point.h"
#include "krado/vector.h"
#include "krado/geom_curve.h"
#include "TopoDS_Face.hxx"
#include "Geom_Surface.hxx"
#include <vector>

namespace krado {

class Model;

class GeomSurface {
public:
    explicit GeomSurface(const TopoDS_Face & face);

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Location in 3D space corresponding to the parametrical position
    Point point(double u, double v) const;

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

    /// Get curves bounding this surface
    ///
    /// @return Curves bounding the surface
    std::vector<GeomCurve> curves() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Face face;
    Handle(Geom_Surface) surface;
    /// Surface area
    double surf_area;
};

} // namespace krado
