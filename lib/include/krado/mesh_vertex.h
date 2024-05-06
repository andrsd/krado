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

private:
    const GeomVertex & gvtx;
};

} // namespace krado
