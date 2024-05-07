// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex.h"
#include "krado/consts.h"
#include "krado/point.h"
#include "krado/geom_vertex.h"

namespace krado {

MeshVertex::MeshVertex(const GeomVertex & geom_vertex) :
    MeshVertexAbstract(geom_vertex),
    gvtx_(geom_vertex)
{
}

int
MeshVertex::tag() const
{
    return this->gvtx_.tag();
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
MeshVertex::prescribed_mesh_size_at_vertex() const
{
    return MAX_LC;
}

} // namespace krado
