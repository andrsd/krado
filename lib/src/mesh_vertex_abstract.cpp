// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex_abstract.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/uv_param.h"
#include "krado/point.h"

namespace krado {

// Mesh vertex "serial" number
static int next_num = 0;

MeshVertexAbstract::MeshVertexAbstract(const GeomShape & geom_shape) :
    gid_(0),
    geom_shape_(geom_shape),
    num_(++next_num)
{
}

int
MeshVertexAbstract::global_id() const
{
    return this->gid_;
}

void
MeshVertexAbstract::set_global_id(int id)
{
    this->gid_ = id;
}

const GeomShape &
MeshVertexAbstract::geom_shape() const
{
    return this->geom_shape_;
}

} // namespace krado
