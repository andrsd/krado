// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_vertex_abstract.h"

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

Span<const Ptr<MeshSurface>>
MeshVolume::surfaces() const
{
    return this->mesh_surfaces_;
}

Span<Ptr<MeshSurface>>
MeshVolume::surfaces()
{
    return this->mesh_surfaces_;
}

Span<const MeshElement>
MeshVolume::tetrahedra() const
{
    return this->tetras_;
}

void
MeshVolume::add_tetra(const std::array<Ptr<MeshVertexAbstract>, 4> & tetra)
{
    MeshElement tet4(ElementType::TETRA4, { tetra[0], tetra[1], tetra[2], tetra[3] });
    this->tetras_.emplace_back(tet4);
}

Scheme3D &
MeshVolume::scheme()
{
    if (this->scheme_ == nullptr)
        throw Exception("No scheme assigned on volume {}", id());
    return *this->scheme_.get();
}

} // namespace krado
