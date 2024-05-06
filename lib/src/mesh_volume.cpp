#include "krado/mesh_volume.h"
#include "krado/exception.h"

namespace krado {

MeshVolume::MeshVolume(const GeomVolume & gvolume,
                       const std::vector<MeshSurface *> & mesh_surfaces) :
    gvolume(gvolume),
    mesh_surfaces(mesh_surfaces),
    meshed(false)
{
    auto & mpars = meshing_parameters();
    mpars.set<std::string>("scheme") = "auto";
    mpars.set<int>("marker") = 0;
}

const GeomVolume &
MeshVolume::geom_volume() const
{
    return this->gvolume;
}

const std::vector<MeshSurface *> &
MeshVolume::surfaces() const
{
    return this->mesh_surfaces;
}

bool
MeshVolume::is_meshed() const
{
    return this->meshed;
}

void
MeshVolume::set_meshed()
{
    this->meshed = true;
}

} // namespace krado
