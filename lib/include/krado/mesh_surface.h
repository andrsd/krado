// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_vertex.h"
#include "krado/mesh_element.h"
#include "krado/meshing_parameters.h"
#include <vector>

namespace krado {

class GeomSurface;
class MeshVertexAbstract;
class MeshCurveVertex;
class MeshSurfaceVertex;
class MeshCurve;

class MeshSurface : public MeshingParameters {
public:
    MeshSurface(const GeomSurface & gcurve, const std::vector<Ptr<MeshCurve>> & mesh_curves);

    /// Get the unique identifier of the surface.
    ///
    /// @return The unique identifier of the surface.
    int id() const;

    /// Get geometrical surface associated with this surface
    ///
    /// @return Geometrical surface associated with this surface
    [[nodiscard]] const GeomSurface & geom_surface() const;

    /// Get curves bounding this surface
    [[nodiscard]] const std::vector<Ptr<MeshCurve>> & curves() const;

    /// Get vertices on this surface
    ///
    /// @return Surface vertices
    [[nodiscard]] const std::vector<Ptr<MeshVertexAbstract>> & all_vertices() const;

    [[nodiscard]] std::vector<Ptr<MeshVertexAbstract>> & all_vertices();

    /// Get (internal) vertices on the surface
    ///
    /// @return Vertices on the surface
    [[nodiscard]] const std::vector<Ptr<MeshSurfaceVertex>> & surface_vertices() const;

    [[nodiscard]] std::vector<Ptr<MeshSurfaceVertex>> & surface_vertices();

    /// Get triangles on this surface
    ///
    /// @return Triangles on this surface
    [[nodiscard]] const std::vector<MeshElement> & triangles() const;

    [[nodiscard]] std::vector<MeshElement> & triangles();

    /// Add vertex
    ///
    /// @param vertex Vertex to add
    void add_vertex(Ptr<MeshVertex> vertex);
    void add_vertex(Ptr<MeshCurveVertex> vertex);
    void add_vertex(Ptr<MeshSurfaceVertex> vertex);

    /// Add new triangle
    ///
    /// @param tri Local vertex indices
    void add_triangle(const std::array<Ptr<MeshVertexAbstract>, 3> & tri);

    /// Add new quadrangle
    ///
    /// @param quad Local vertices
    void add_quadrangle(const std::array<Ptr<MeshVertexAbstract>, 4> & quad);

    void add_element(MeshElement tri);

    /// Reserve memory for vertices and triangles
    void reserve_mem(std::size_t n_vtxs, std::size_t n_tris);

    void set_triangles(const std::vector<MeshElement> & new_tris);

    const std::vector<MeshElement> & elements() const;

    void remove_all_triangles();

    void delete_mesh();

private:
    const GeomSurface & gsurface_;
    /// Mesh curves bounding this surface
    std::vector<Ptr<MeshCurve>> mesh_curves_;
    /// All vertices on this surface
    std::vector<Ptr<MeshVertexAbstract>> vtxs_;
    /// Surface vertices
    std::vector<Ptr<MeshSurfaceVertex>> surf_vtxs_;
    /// Triangles
    std::vector<MeshElement> tris_;
    /// Quadrangles
    std::vector<MeshElement> quads_;
};

} // namespace krado
