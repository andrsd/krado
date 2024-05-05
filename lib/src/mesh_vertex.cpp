#include "krado/mesh_vertex.h"

namespace krado::mesh {

MeshVertex::MeshVertex(const geo::GeomVertex & geom_vertex) : gvtx(geom_vertex) {}

const geo::GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx;
}

} // namespace krado::mesh
