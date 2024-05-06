#pragma once

#include "krado/geom_vertex.h"
#include "krado/size_parameters.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshVertex : public SizeParameters, public MeshingParameters {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    /// Get geometrical vertex associated with this vertex
    ///
    /// @return Geometrical vertex associated with this vertex
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
