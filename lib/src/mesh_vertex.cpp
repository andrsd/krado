// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : MeshVertexAbstract(), gvtx(geom_vertex) {}

const GeomVertex &
MeshVertex::geom_vertex() const
{
    return this->gvtx;
}

Point
MeshVertex::point() const
{
    return Point(this->gvtx.x(), this->gvtx.y(), this->gvtx.z());
}

} // namespace krado
