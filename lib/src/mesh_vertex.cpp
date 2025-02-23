// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) :
    MeshVertexAbstract(geom_vertex),
    gvtx_(geom_vertex)
{
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

} // namespace krado
