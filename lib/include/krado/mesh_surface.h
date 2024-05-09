// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_element.h"
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
    const GeomSurface & geom_surface() const;

    /// Get curves bounding this surface
    const std::vector<MeshCurve *> & curves() const;

    /// Get vertices on this surface
    ///
    /// @return Surface vertices
    const std::vector<MeshVertexAbstract *> & all_vertices() const;

    /// Get triangles on this surface
    ///
    /// @return Triangles on this surface
    const std::vector<MeshElement> & triangles() const;

    /// Add vertex
    ///
    /// @param vertex Vertex to add
    void add_vertex(MeshVertex * vertex);
    void add_vertex(MeshCurveVertex * vertex);
    void add_vertex(MeshSurfaceVertex * vertex);

    /// Add new triangle
    ///
    /// @param idx1 Local index of the first vertex
    /// @param idx2 Local index of the second vertex
    /// @param idx3 Local index of the third vertex
    void add_triangle(int idx1, int idx2, int idx3);

    /// Check if the surface is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark surface as meshed
    void set_meshed();

private:
    const GeomSurface & gsurface;
    /// Mesh curves bounding this surface
    std::vector<MeshCurve *> mesh_curves;
    /// All vertices on this surface
    std::vector<MeshVertexAbstract *> vtxs;
    /// Triangles
    std::vector<MeshElement> tris;
    /// Flag indicating if the surface is meshed
    bool meshed;
};

} // namespace krado
