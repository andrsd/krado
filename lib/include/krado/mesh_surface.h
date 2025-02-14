// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/element.h"
#include "krado/meshing_parameters.h"

namespace krado {

class GeomSurface;
class MeshVertexAbstract;
class MeshVertex;
class MeshCurveVertex;
class MeshSurfaceVertex;
class MeshCurve;

class MeshSurface : public MeshingParameters {
public:
    MeshSurface(const GeomSurface & gcurve, const std::vector<MeshCurve *> & mesh_curves);

    /// Get geometrical surface associated with this surface
    ///
    /// @return Geometrical surface associated with this surface
    [[nodiscard]] const GeomSurface & geom_surface() const;

    /// Get curves bounding this surface
    [[nodiscard]] const std::vector<MeshCurve *> & curves() const;

    /// Get vertices on this surface
    ///
    /// @return Surface vertices
    [[nodiscard]] const std::vector<MeshVertexAbstract *> & all_vertices() const;

    /// Get (internal) vertices on the surface
    ///
    /// @return Vertices on the surface
    [[nodiscard]] const std::vector<MeshSurfaceVertex *> & surface_vertices() const;

    /// Get triangles on this surface
    ///
    /// @return Triangles on this surface
    [[nodiscard]] const std::vector<Element> & triangles() const;

    /// Add vertex
    ///
    /// @param vertex Vertex to add
    void add_vertex(MeshVertex * vertex);
    void add_vertex(MeshCurveVertex * vertex);
    void add_vertex(MeshSurfaceVertex * vertex);

    /// Add new triangle
    ///
    /// @param tri Local vertex indices
    void add_triangle(const std::array<std::size_t, 3> & tri);

    /// Reserve memory for vertices and triangles
    void reserve_mem(std::size_t n_vtxs, std::size_t n_tris);

private:
    const GeomSurface & gsurface;
    /// Mesh curves bounding this surface
    std::vector<MeshCurve *> mesh_curves;
    /// All vertices on this surface
    std::vector<MeshVertexAbstract *> vtxs;
    /// Surface vertices
    std::vector<MeshSurfaceVertex *> surf_vtxs;
    /// Triangles
    std::vector<Element> tris;
};

} // namespace krado
