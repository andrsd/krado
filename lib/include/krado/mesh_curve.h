// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_element.h"
#include "krado/meshing_parameters.h"
#include "krado/ptr.h"
#include <vector>
#include <optional>

namespace krado {

class GeomVertex;
class GeomCurve;
class MeshVertex;
class MeshCurveVertex;

class MeshCurve : public MeshingParameters {
public:
    MeshCurve(const GeomCurve & gcurve, Ptr<MeshVertex> v1, Ptr<MeshVertex> v2);

    /// Get the unique identifier of the curve.
    ///
    /// @return The unique identifier of the curve.
    int id() const;

    /// Get geometrical curve associated with this curve
    ///
    /// @return Geometrical curve associated with this curve
    [[nodiscard]] const GeomCurve & geom_curve() const;

    /// Get vertices on this curve
    ///
    /// @return Curve vertices
    [[nodiscard]] const std::vector<Ptr<MeshVertexAbstract>> & all_vertices() const;

    ///
    [[nodiscard]] const std::vector<Ptr<MeshVertex>> & bounding_vertices() const;

    /// Add vertex
    ///
    /// @param vertex Vertex to add
    void add_vertex(Ptr<MeshVertex> vertex);

    /// Add curve vertex
    ///
    /// @param vertex Curve vertex to add
    void add_vertex(Ptr<MeshCurveVertex> vertex);

    /// Get (internal) vertices on the curve
    ///
    /// @return Vertices on the curve
    [[nodiscard]] const std::vector<Ptr<MeshCurveVertex>> & curve_vertices() const;

    /// Get (internal) vertices on the curve
    ///
    /// @return Vertices on the curve
    [[nodiscard]] std::vector<Ptr<MeshCurveVertex>> & curve_vertices();

    /// Add new curve segment
    ///
    /// @param seg Local vertex indices
    void add_segment(const std::array<Ptr<MeshVertexAbstract>, 2> & seg);

    /// Get curve segments
    ///
    /// @return Curse segments using vertex indexing local to this edge
    [[nodiscard]] const std::vector<MeshElement> & segments() const;

    bool is_mesh_degenerated() const;

    void set_too_small(bool value);

    /// Get the mesh size at the vertex.
    ///
    /// @return The mesh size at the vertex.
    double mesh_size() const;

    /// Set the mesh size at the vertex.
    ///
    /// @param size The new mesh size.
    void set_mesh_size(double size);

    /// Get mesh size at parameter (interpolates between mesh size at bounding vertices)
    ///
    /// @param u Parameter value
    /// @return Mesh size at parameter
    double mesh_size_at_param(double u) const;

private:
    const GeomCurve & gcurve_;
    /// All vertices on this curve
    std::vector<Ptr<MeshVertexAbstract>> vtxs_;
    /// Bounding vertices
    std::vector<Ptr<MeshVertex>> bnd_vtxs_;
    /// Vertices on the curve (excluding the bounding vertices)
    std::vector<Ptr<MeshCurveVertex>> curve_vtx_;
    /// Segments of this curve, using vertex indexing local to this edge
    std::vector<MeshElement> segs_;
    ///
    bool too_smoll;
    /// Mesh size for the edge.
    std::optional<double> mesh_size_;
};

} // namespace krado
