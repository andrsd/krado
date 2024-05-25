// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface_vertex.h"
#include "krado/geom_surface.h"

namespace krado {

MeshSurfaceVertex::MeshSurfaceVertex(const GeomSurface & geom_surface, double u, double v) :
    MeshVertexAbstract(geom_surface),
    gsurface_(geom_surface),
    u_(u),
    v_(v)
{
    this->phys_pt_ = geom_surface.point(u, v);
}

const GeomSurface &
MeshSurfaceVertex::geom_surface() const
{
    return this->gsurface_;
}

UVParam
MeshSurfaceVertex::parameter() const
{
    return { this->u_, this->v_ };
}

Point
MeshSurfaceVertex::point() const
{
    return this->phys_pt_;
}

} // namespace krado
