#include "krado/mesh/vertex.h"

namespace krado::mesh {

Vertex::Vertex(const geo::Vertex & geom_vertex) : gvtx(geom_vertex) {}

const geo::Vertex &
Vertex::geom_vertex() const
{
    return this->gvtx;
}

} // namespace krado::mesh
