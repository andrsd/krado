#pragma once

#include "krado/geom_vertex.h"

namespace krado {

class MeshVertex {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    const GeomVertex & geom_vertex() const;

private:
    const GeomVertex & gvtx;
};

} // namespace krado
