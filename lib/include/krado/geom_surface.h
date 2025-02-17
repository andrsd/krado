// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/geom_curve.h"
#include "TopoDS_Face.hxx"
#include "Geom_Surface.hxx"
#include "GeomAPI_ProjectPointOnSurf.hxx"
#include <vector>

namespace krado {

class GeomModel;

class GeomSurface : public GeomShape {
public:
    explicit GeomSurface(const TopoDS_Face & face);
    GeomSurface(const GeomSurface & other);
    GeomSurface(GeomSurface && other);

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Location in 3D space corresponding to the parametrical position
    [[nodiscard]] Point point(double u, double v) const;

    /// Get normal vector at parametrical location
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Normal vector at location (u, v)
    [[nodiscard]] Vector normal(double u, double v) const;

    /// Compute first derivative at parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return First derivative
    [[nodiscard]] std::tuple<Vector, Vector> d1(double u, double v) const;

    /// Get area of the surface
    ///
    /// @return Area of the surface
    [[nodiscard]] double area() const;

    /// Get range of the parameter
    ///
    /// @return Range as a tuple [lower, upper]
    [[nodiscard]] std::tuple<double, double> param_range(int i) const;

    /// Get curves bounding this surface
    ///
    /// @return Curves bounding the surface
    [[nodiscard]] std::vector<GeomCurve> curves() const;

    /// Get parameter on the surface from a physical location
    ///
    /// @param pt Physical location
    /// @return Parameters (u, v)
    [[nodiscard]] std::tuple<double, double> parameter_from_point(const Point & pt) const;

    /// Find nearest point
    ///
    /// @param pt Physical point
    /// @return Point on the curve, nearest to `pt`
    [[nodiscard]] Point nearest_point(const Point & pt) const;

    /// Check if point is on the curve
    ///
    /// @param pt Point to investigate
    /// @return `true` if the point is on the curve, `false` otherwise
    [[nodiscard]] bool contains_point(const Point & pt) const;

    operator const TopoDS_Shape &() const;

private:
    std::tuple<bool, double, double> project(const Point & pt) const;

    TopoDS_Face face_;
    Handle(Geom_Surface) surface_;
    /// Surface area
    double surf_area_;
    double umin_, umax_;
    double vmin_, vmax_;
    /// Needs to be a pointer, because GeomSurface must be movable
    mutable GeomAPI_ProjectPointOnSurf proj_pt_on_surface_;
};

} // namespace krado
