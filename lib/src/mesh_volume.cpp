#include "krado/mesh_volume.h"
#include "krado/exception.h"

namespace krado {

MeshVolume::MeshVolume(const GeomVolume & gvolume,
                       const std::vector<const MeshSurface *> & mesh_surfaces) :
    gvolume(gvolume),
    mesh_surfaces(mesh_surfaces)
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

const std::vector<const MeshSurface *> &
MeshVolume::surfaces() const
{
    return this->mesh_surfaces;
}

} // namespace krado
