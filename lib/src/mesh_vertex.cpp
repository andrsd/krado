// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : MeshVertexAbstract(), gvtx_(geom_vertex) {}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx_;
}

Point
MeshVertex::point() const
{
    return Point(this->gvtx_.x(), this->gvtx_.y(), this->gvtx_.z());
}

} // namespace krado
