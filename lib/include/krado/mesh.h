// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_point.h"
#include "krado/point.h"
#include "krado/scheme_factory.h"
#include "krado/bounding_box_3d.h"
#include "krado/transform.h"
#include <map>

namespace krado {

class GeomModel;

class Mesh {
public:
    /// Construct empty mesh
    Mesh();

    /// Construct mesh from geometric model
    ///
    /// @param model Geometric model
    Mesh(const GeomModel & model);

    /// Construct mesh from set of points and elements (useful for reading meshes from files)
    ///
    /// @param points Points
    /// @param elements Elements
    Mesh(std::vector<Point> points, std::vector<MeshElement> elements);

    /// Vertex
    const MeshVertex & vertex(int id) const;
    MeshVertex & vertex(int id);

    /// Vertices
    const std::map<int, MeshVertex> & vertices() const;

    /// Curve
    const MeshCurve & curve(int id) const;
    MeshCurve & curve(int id);

    /// Curves
    const std::map<int, MeshCurve> & curves() const;

    /// Surface
    const MeshSurface & surface(int id) const;
    MeshSurface & surface(int id);

    /// Surfaces
    const std::map<int, MeshSurface> & surfaces() const;

    /// Volume
    const MeshVolume & volume(int id) const;
    MeshVolume & volume(int id);

    /// Volumes
    const std::map<int, MeshVolume> & volumes() const;

    /// Create vertex mesh
    void mesh_vertex(int id);
    void mesh_vertex(MeshVertex & vertex);

    /// Create curve mesh
    void mesh_curve(int id);
    void mesh_curve(MeshCurve & curve);

    /// Create surface mesh
    void mesh_surface(int id);
    void mesh_surface(MeshSurface & surface);

    /// Create volume mesh
    void mesh_volume(int id);
    void mesh_volume(MeshVolume & volume);

    /// Get mesh points
    ///
    /// @return Mesh points
    const std::vector<Point> & points() const;

    /// Get elements
    ///
    /// @return Mesh elements
    const std::vector<MeshElement> & elements() const;

    ///
    void number_points();

    ///
    void build_elements();

    /// Get mesh bounding box
    BoundingBox3D bounding_box() const;

    /// Scale mesh by a factor (isotropic)
    ///
    /// @param factor Scaling factor
    /// @return Scaled mesh
    Mesh scaled(double factor) const;

    /// Scale mesh by a factor (unisotropic)
    ///
    /// @param factor_x Scaling factor in x-direction
    /// @param factor_y Scaling factor in y-direction
    /// @param factor_z Scaling factor in z-direction
    /// @return Scaled mesh
    Mesh scaled(double factor_x, double factor_y, double factor_z = 1.) const;

    /// Translate mesh
    ///
    /// @param tx Translation in x-direction
    /// @param ty Translation in y-direction
    /// @param tz Translation in z-direction
    /// @return Translated mesh
    Mesh translated(double tx, double ty = 0., double tz = 0.) const;

    /// Transform mesh
    ///
    /// @param tr Transformation
    /// @return Transformed mesh
    Mesh transformed(const Trsf & tr) const;

    /// Add another mesh to this mesh
    ///
    /// @param other Mesh to add
    void add(const Mesh & other);

protected:
    void build_1d_elements();
    void build_2d_elements();

    template <typename T, typename U>
    T &
    get_scheme(U entity) const
    {
        return dynamic_cast<T &>(entity.scheme());
    }

    /// Assign a new global ID to a vertex
    ///
    /// @param vertex Vertex to assign global ID to
    void assign_gid(MeshVertex & vertex);

    /// Assign a new global ID to a curve vertex
    ///
    /// @param vertex Curve vertex to assign global ID to
    void assign_gid(MeshCurveVertex & vertex);

    /// Assign a new global ID to a surface vertex
    ///
    /// @param vertex Surface vertex to assign global ID to
    void assign_gid(MeshSurfaceVertex & vertex);

    void add_mesh_point(Point & mpnt);

private:
    void initialize(const GeomModel & model);

    std::map<int, MeshVertex> vtxs;
    std::map<int, MeshCurve> crvs;
    std::map<int, MeshSurface> surfs;
    std::map<int, MeshVolume> vols;

    SchemeFactory & scheme_factory;

    /// Mesh points
    std::vector<Point> pnts;
    /// Mesh elements. This is indexing the `pnts` vector
    std::vector<MeshElement> elems;

    /// Global ID counter
    int gid_ctr;
    /// Bounding box around the mesh that is being exported
    BoundingBox3D exp_bbox;
};

} // namespace krado
