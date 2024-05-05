#pragma once

#include "krado/geom_vertex.h"

namespace krado::mesh {

class MeshVertex {
public:
    MeshVertex(const geo::GeomVertex & geom_vertex);

    const geo::GeomVertex & geom_vertex() const;

private:
    const geo::GeomVertex & gvtx;
};

} // namespace krado::mesh
