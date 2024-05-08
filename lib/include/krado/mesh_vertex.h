#pragma once

#include "krado/geom_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/meshing_parameters.h"
#include "krado/point.h"

namespace krado {

class MeshVertex : public MeshVertexAbstract, public MeshingParameters {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    /// Get geometrical vertex associated with this vertex
    ///
    /// @return Geometrical vertex associated with this vertex
    const GeomVertex & geom_vertex() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    Point point() const override;

    /// Check if the vertex is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark vertex as meshed
    void set_meshed();

private:
    const GeomVertex & gvtx;
    /// Flag indicating if the vertex is meshed
    bool meshed;
};

} // namespace krado
