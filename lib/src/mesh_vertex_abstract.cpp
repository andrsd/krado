// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex_abstract.h"

namespace krado {

MeshVertexAbstract::MeshVertexAbstract(const GeomShape & geom_shape) : geom_shape_(geom_shape) {}

const GeomShape &
MeshVertexAbstract::geom_shape() const
{
    return this->geom_shape_;
}

} // namespace krado
