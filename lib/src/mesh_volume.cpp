// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"

namespace krado {

MeshVolume::MeshVolume(ShapeID id,
                       const GeomVolume & gvolume,
                       const std::vector<Ptr<MeshSurface>> & mesh_surfaces) :
    id_(id),
    gvolume_(gvolume),
    mesh_surfaces_(mesh_surfaces)
{
}

MeshVolume::~MeshVolume() = default;

ShapeID
MeshVolume::id() const
{
    return this->id_;
}

const GeomVolume &
MeshVolume::geom_volume() const
{
    return this->gvolume_;
}

const std::vector<Ptr<MeshSurface>> &
MeshVolume::surfaces() const
{
    return this->mesh_surfaces_;
}

Scheme3D &
MeshVolume::scheme()
{
    return *this->scheme_.get();
}

} // namespace krado
