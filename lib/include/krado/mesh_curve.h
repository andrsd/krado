#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include <vector>

namespace krado {

class MeshCurve {
public:
    enum VertexType {
        FIRST_VERTEX = -1,
        LAST_VERTEX = -2
    };

    MeshCurve(const GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2);

    /// Get geometrical curve associated with this curve
    ///
    /// @return Geometrical curve associated with this curve
    const GeomCurve & geom_curve() const;

    const MeshVertex & vertex(int id) const;

    /// Get curve marker
    ///
    /// @return Curve marker
    int marker() const;

    /// Set curve marker
    ///
    /// @param marker New curve marker
    void set_marker(int marker);

    /// Add internal vertex
    ///
    /// @param curve_vertex (Internal) curve vertex to add
    void add_curve_vertex(const MeshCurveVertex & curve_vertex);

    /// Get (internal) vertices on the curve
    ///
    /// @return Vertices on the curve
    const std::vector<MeshCurveVertex> & curve_vertices() const;

    /// Add new curve segment
    ///
    /// @param idx1 Local index of the first vertex
    /// @param idx2 Local index of the second vertex
    void add_curve_segment(int idx1, int idx2);

    /// Get curve segments
    ///
    /// @return Curse segments using vertex indexing local to this edge
    const std::vector<MeshElement> & curve_segments() const;

private:
    const GeomCurve & gcurve;
    /// Start vertex
    const MeshVertex * v1;
    /// End vertex
    const MeshVertex * v2;
    /// Curve marker
    int curve_marker;
    /// Vertices on the curve (excluding the bounding vertices, i.e. `v1`, `v2`)
    std::vector<MeshCurveVertex> curve_vtx;
    /// Segments of this curve, using vertex indexing local to this edge
    std::vector<MeshElement> curve_segs;
};

} // namespace krado
