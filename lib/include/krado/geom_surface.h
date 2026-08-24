// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/geom_curve.h"
#include "TopoDS_Face.hxx"
#include "Geom_Surface.hxx"
#include "GeomAPI_ProjectPointOnSurf.hxx"
#include <vector>

namespace krado {

class GeomModel;
class Wire;
class UVParam;
class Point;
class Vector;

class GeomSurface : public GeomShape {
public:
    enum class SurfaceType : u8 {
        Plane,
        Spherical,
        Cylindrical,
        BSpline,
        Bezier,
        Conical,
        Unknown
    };

    explicit GeomSurface(const TopoDS_Face & face);
    GeomSurface(const GeomSurface & other);
    GeomSurface(GeomSurface && other);

    int dim() const final;

    /// Get surface type
    ///
    /// @return Surface type
    [[nodiscard]] SurfaceType type() const;

    /// Get physical location from parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Location in 3D space corresponding to the parametrical position
    [[nodiscard]] Point point(UVParam uv) const;

    /// Get normal vector at parametrical location
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return Normal vector at location (u, v)
    [[nodiscard]] Vector normal(UVParam param) const;

    /// Compute first derivative at parametrical position
    ///
    /// @param u Parameter specifying location
    /// @param v Parameter specifying location
    /// @return First derivative
    [[nodiscard]] std::tuple<Vector, Vector> d1(UVParam param) const;

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
    [[nodiscard]] UVParam parameter_from_point(Point pt) const;

    /// Find nearest point
    ///
    /// @param pt Physical point
    /// @return Point on the curve, nearest to `pt`
    [[nodiscard]] Point nearest_point(Point pt) const;

    ///
    std::tuple<Point, UVParam> closest_point(Point qp, UVParam uv) const;

    /// Check if point is on the curve
    ///
    /// @param pt Point to investigate
    /// @return `true` if the point is on the curve, `false` otherwise
    [[nodiscard]] bool contains_point(Point pt) const;

    /// Get mesh size at given surface parameter
    ///
    /// @param par Surface parameter (u, v)
    /// @return Mesh size at the parameter
    [[nodiscard]] double mesh_size_at_param(UVParam par) const;

    operator const TopoDS_Face &() const;

    [[nodiscard]] const Handle(Geom_Surface) & surface_handle() const;

private:
    std::tuple<bool, UVParam> project(Point pt) const;

    Handle(Geom_Surface) surface_;
    SurfaceType surface_type_;
    double umin_, umax_;
    double vmin_, vmax_;
    /// Mesh size for the edge.
    Optional<double> mesh_size_;

    /// Needs to be a pointer, because GeomSurface must be movable
    mutable GeomAPI_ProjectPointOnSurf proj_pt_on_surface_;

public:
    static GeomSurface create(const Wire & wire);

    friend class MeshSurface;
};

/// Check that the surface is circular
///
/// @param surface The surface to check
/// @return `true` if the surface is circular, `false` otherwise
bool is_circular_face(const GeomSurface & surface);

/// Get cylinder radius
///
/// @param surface Surface to investigate
/// @return Cylinder radius
double get_radius(const GeomSurface & surface);

/// Reparametrize the point onto the given surface
///
/// @param surface Geometric surface to reparametrize onto
/// @param curve Geometric curve
/// @param u Curve parameter
UVParam reparam_on_surface(const GeomSurface & surface, const GeomCurve & curve, double u);

} // namespace krado

std::ostream & operator<<(std::ostream & stream, const krado::GeomSurface & srf);
std::ostream & operator<<(std::ostream & stream, const krado::GeomSurface::SurfaceType & type);

// fmt formatters

template <>
struct fmt::formatter<krado::GeomSurface::SurfaceType> {
    constexpr auto
    parse(format_parse_context & ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto
    format(const krado::GeomSurface::SurfaceType & obj, FormatContext & ctx) const
        -> decltype(ctx.out())
    {
        switch (obj) {
        case krado::GeomSurface::SurfaceType::Plane:
            return fmt::format_to(ctx.out(), "plane");
        case krado::GeomSurface::SurfaceType::Spherical:
            return fmt::format_to(ctx.out(), "spherical");
        case krado::GeomSurface::SurfaceType::Cylindrical:
            return fmt::format_to(ctx.out(), "cylindrical");
        case krado::GeomSurface::SurfaceType::BSpline:
            return fmt::format_to(ctx.out(), "b-spline");
        case krado::GeomSurface::SurfaceType::Bezier:
            return fmt::format_to(ctx.out(), "bezier");
        case krado::GeomSurface::SurfaceType::Conical:
            return fmt::format_to(ctx.out(), "conical");
        default:
            return fmt::format_to(ctx.out(), "unknown");
        }
    }
};
