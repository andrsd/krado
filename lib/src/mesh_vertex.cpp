#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : gvtx(geom_vertex), vertex_marker(0) {}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx;
}

int
MeshVertex::marker() const
{
    return this->vertex_marker;
}

void
MeshVertex::set_marker(int marker)
{
    this->vertex_marker = marker;
}

} // namespace krado
