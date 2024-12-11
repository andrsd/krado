// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/element.h"
#include "krado/meshing_parameters.h"
#include <vector>

namespace krado {

class GeomCurve;
class MeshVertexAbstract;
class MeshVertex;
class MeshCurveVertex;

class MeshCurve : public MeshingParameters {
public:
    MeshCurve(const GeomCurve & gcurve, MeshVertex * v1, MeshVertex * v2);

    /// Get geometrical curve associated with this curve
    ///
    /// @return Geometrical curve associated with this curve
    const GeomCurve & geom_curve() const;

    /// Get vertices on this curve
    ///
    /// @return Curve vertices
    const std::vector<MeshVertexAbstract *> & all_vertices() const;

    ///
    const std::vector<MeshVertex *> & bounding_vertices() const;

    /// Add vertex
    ///
    /// @param vertex Vertex to add
    void add_vertex(MeshVertex * vertex);

    /// Add curve vertex
    ///
    /// @param vertex Curve vertex to add
    void add_vertex(MeshCurveVertex * vertex);

    /// Get (internal) vertices on the curve
    ///
    /// @return Vertices on the curve
    const std::vector<MeshCurveVertex *> & curve_vertices() const;

    /// Get (internal) vertices on the curve
    ///
    /// @return Vertices on the curve
    std::vector<MeshCurveVertex *> & curve_vertices();

    /// Add new curve segment
    ///
    /// @param seg Local vertex indices
    void add_segment(const std::array<std::size_t, 2> & seg);

    /// Get curve segments
    ///
    /// @return Curse segments using vertex indexing local to this edge
    const std::vector<Element> & segments() const;

private:
    const GeomCurve & gcurve;
    /// All vertices on this curve
    std::vector<MeshVertexAbstract *> vtxs;
    /// Bounding vertices
    std::vector<MeshVertex *> bnd_vtxs;
    /// Vertices on the curve (excluding the bounding vertices)
    std::vector<MeshCurveVertex *> curve_vtx;
    /// Segments of this curve, using vertex indexing local to this edge
    std::vector<Element> segs;
};

} // namespace krado
