#include "krado/mesh_volume.h"
#include "krado/exception.h"

namespace krado {

MeshVolume::MeshVolume(const GeomVolume & gvolume,
                       const std::vector<const MeshSurface *> & mesh_surfaces) :
    gvolume(gvolume),
    volume_marker(0),
    mesh_surfaces(mesh_surfaces)
{
}

const GeomVolume &
MeshVolume::geom_volume() const
{
    return this->gvolume;
}

int
MeshVolume::marker() const
{
    return this->volume_marker;
}

void
MeshVolume::set_marker(int marker)
{
    this->volume_marker = marker;
}

const std::vector<const MeshSurface *> &
MeshVolume::surfaces() const
{
    return this->mesh_surfaces;
}

} // namespace krado
