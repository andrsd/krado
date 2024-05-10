// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_volume.h"
#include "krado/exception.h"
#include "krado/scheme.h"

namespace krado {

MeshVolume::MeshVolume(const GeomVolume & gvolume,
                       const std::vector<MeshSurface *> & mesh_surfaces) :
    gvolume(gvolume),
    mesh_surfaces(mesh_surfaces),
    meshed(false)
{
    set<int>("marker", 0);
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
