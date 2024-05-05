#pragma once

#include "krado/geom_vertex.h"

namespace krado {

class MeshVertex {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    const GeomVertex & geom_vertex() const;

    /// Get vertex marker
    ///
    /// @return Vertex marker
    int marker() const;

    /// Set vertex marker
    ///
    /// @param marker New vertex marker
    void set_marker(int marker);

private:
    const GeomVertex & gvtx;
    /// Vertex marker
    int vertex_marker;
};

} // namespace krado
