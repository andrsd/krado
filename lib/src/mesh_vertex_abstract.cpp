// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex_abstract.h"

namespace krado {

namespace {

int vertex_counter = 0;

}

MeshVertexAbstract::MeshVertexAbstract(const GeomShape & geom_shape) :
    geom_shape_(geom_shape),
    num_(vertex_counter++)
{
}

const GeomShape &
MeshVertexAbstract::geom_shape() const
{
    return this->geom_shape_;
}

int
MeshVertexAbstract::num() const
{
    return this->num_;
}

} // namespace krado
