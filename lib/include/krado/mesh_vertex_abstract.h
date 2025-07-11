// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <tuple>

namespace krado {

class GeomShape;
class Point;
class GeomCurve;
class GeomSurface;
class UVParam;

class MeshVertexAbstract {
public:
    MeshVertexAbstract(const GeomShape & geom_shape);
    virtual ~MeshVertexAbstract() = default;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] virtual Point point() const = 0;

    /// Get global ID
    ///
    /// @return Global ID of this vertex
    [[nodiscard]] int global_id() const;

    /// Set global ID
    ///
    /// @param id New ID to assign
    void set_global_id(int id);

    /// Geometrical shape associated with this vertex
    ///
    /// @return Geometrical shape associated with this vertex
    const GeomShape & geom_shape() const;

    ///
    int num() const;

    void set_num(int num);

private:
    /// Global ID of this vertex
    int gid_;
    /// Geometrical shape associated with this vertex
    const GeomShape & geom_shape_;
    int num_;

public:
    struct PtrLessThan {
        bool
        operator()(const MeshVertexAbstract * v1, const MeshVertexAbstract * v2) const
        {
            return v1->num_ < v2->num_;
        }
    };
};

/// Reparametrize a mesh vertex on a curve
///
/// @param v Mesh vertex to reparametrize
/// @param gcurve Geometrical curve to reparametrize on
std::tuple<double, bool> reparam_mesh_vertex_on_curve(const MeshVertexAbstract * v,
                                                      const GeomCurve & gcurve);

UVParam
reparam_on_surface(const GeomSurface & gsurface, const GeomCurve & gcurve, double t, int dir);

/// Reparametrize a mesh vertex on a face
///
/// @param v Mesh vertex to reparametrize
/// @param geom_surface Geometrical surface to reparametrize on
/// @param on_surface Whether to reparametrize on the surface or not
/// @param fail_on_seam Whether to fail on seam or not
/// @return (uv_param, success)
///         - uv_param parameterical coordinates on the surface
///         - success flag indicating the the operation succeeded
std::tuple<UVParam, bool> reparam_mesh_vertex_on_surface(const MeshVertexAbstract * v,
                                                         const GeomSurface & geom_surface,
                                                         bool on_surface = true,
                                                         bool fail_on_seam = true,
                                                         int dir = 1);

} // namespace krado
