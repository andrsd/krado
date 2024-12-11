// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_volume.h"
#include "krado/exception.h"
#include "krado/scheme.h"

namespace krado {

MeshVolume::MeshVolume(const GeomVolume & gvolume,
                       const std::vector<MeshSurface *> & mesh_surfaces) :
    gvolume(gvolume),
    mesh_surfaces(mesh_surfaces)
{
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

} // namespace krado
