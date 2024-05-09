// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/meshing_parameters.h"
#include <vector>

namespace krado {

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

    /// Add internal vertex
    ///
    /// @param curve_vertex (Internal) curve vertex to add
    void add_vertex(MeshVertex * vertex);

    /// Add internal vertex
    ///
    /// @param curve_vertex (Internal) curve vertex to add
    void add_curve_vertex(MeshCurveVertex * curve_vertex);

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
    /// @param idx1 Local index of the first vertex
    /// @param idx2 Local index of the second vertex
    void add_curve_segment(int idx1, int idx2);

    /// Get curve segments
    ///
    /// @return Curse segments using vertex indexing local to this edge
    const std::vector<MeshElement> & curve_segments() const;

    /// Check if the curve is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark curve as meshed
    void set_meshed();

private:
    const GeomCurve & gcurve;
    /// All vertices on this curve
    std::vector<MeshVertexAbstract *> vtxs;
    /// Bounding vertices
    std::vector<MeshVertex *> bnd_vtxs;
    /// Vertices on the curve (excluding the bounding vertices)
    std::vector<MeshCurveVertex *> curve_vtx;
    /// Segments of this curve, using vertex indexing local to this edge
    std::vector<MeshElement> curve_segs;
    /// Flag indicating if the curve is meshed
    bool meshed;
};

} // namespace krado
