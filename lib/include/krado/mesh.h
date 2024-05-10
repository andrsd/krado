// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_point.h"
#include "krado/scheme_factory.h"
#include <map>

namespace krado {

class GeomModel;

class Mesh {
public:
    Mesh();
    Mesh(const GeomModel & model);

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

protected:
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

    std::vector<Point> pnts;
    std::vector<MeshElement> elems;

    /// Global ID counter
    int gid_ctr;
};

} // namespace krado
