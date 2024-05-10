// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) : MeshVertexAbstract(), gvtx(geom_vertex)
{
    set<int>("marker", 0);
}

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

bool
MeshVertex::is_meshed() const
{
    return this->meshed;
}

void
MeshVertex::set_meshed()
{
    this->meshed = true;
}

} // namespace krado
