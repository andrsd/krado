#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : gvtx(geom_vertex)
{
    auto & mpars = meshing_parameters();
    mpars.set<int>("marker") = 0;
}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx;
}

} // namespace krado
