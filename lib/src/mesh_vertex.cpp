#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : gvtx(geom_vertex), gid(0)
{
    auto & mpars = meshing_parameters();
    mpars.set<int>("marker") = 0;
}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx;
}

int
MeshVertex::global_id() const
{
    return this->gid;
}

void
MeshVertex::set_global_id(int id)
{
    this->gid = id;
}

} // namespace krado
