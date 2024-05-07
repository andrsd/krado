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

private:
    const GeomVertex & gvtx;
    int gid;
};

} // namespace krado
