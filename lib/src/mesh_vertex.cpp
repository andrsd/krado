// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"
#include "krado/consts.h"
#include "krado/point.h"
#include "krado/geom_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) :
    MeshVertexAbstract(geom_vertex),
    gvtx_(geom_vertex),
    mesh_size_(MAX_LC)
{
}

int
MeshVertex::id() const
{
    return this->gvtx_.id();
}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx_;
}

Point
MeshVertex::point() const
{
    return this->gvtx_.point();
}

double
MeshVertex::mesh_size() const
{
    return this->mesh_size_;
}

void
MeshVertex::set_mesh_size(double size)
{
    this->mesh_size_ = size;
}

} // namespace krado
