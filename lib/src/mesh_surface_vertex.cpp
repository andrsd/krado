// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface_vertex.h"
#include "krado/geom_surface.h"

namespace krado {

MeshSurfaceVertex::MeshSurfaceVertex(const GeomSurface & geom_surface, double u, double v) :
    MeshVertexAbstract(),
    gsurface(geom_surface),
    u(u),
    v(v)
{
    this->phys_pt = geom_surface.point(u, v);
}

const GeomSurface &
MeshSurfaceVertex::geom_surface() const
{
    return this->gsurface;
}

std::tuple<double, double>
MeshSurfaceVertex::parameter() const
{
    return { this->u, this->v };
}

Point
MeshSurfaceVertex::point() const
{
    return this->phys_pt;
}

} // namespace krado
