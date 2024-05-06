#pragma once

#include "krado/geom_vertex.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshVertex : public MeshingParameters {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    /// Get geometrical vertex associated with this vertex
    ///
    /// @return Geometrical vertex associated with this vertex
    const GeomVertex & geom_vertex() const;

    /// Get global ID
    ///
    /// @return Global ID of this vertex
    int global_id() const;

    /// Set global ID
    ///
    /// @param id New ID to assign
    void set_global_id(int id);

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
    int gid;
};

} // namespace krado
