#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : gvtx(geom_vertex) {}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx;
}

} // namespace krado
