#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/size_parameters.h"
#include "krado/meshing_parameters.h"
#include <vector>

namespace krado {

class MeshCurve : public SizeParameters, public MeshingParameters {
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

    /// Get bounding vertices
    ///
    /// @return Two bounding vertices
    const std::vector<const MeshVertex *> & vertices() const;

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
    /// Bounding vertices
    std::vector<const MeshVertex *> vtxs;
    /// Vertices on the curve (excluding the bounding vertices)
    std::vector<MeshCurveVertex> curve_vtx;
    /// Segments of this curve, using vertex indexing local to this edge
    std::vector<MeshElement> curve_segs;
};

} // namespace krado
