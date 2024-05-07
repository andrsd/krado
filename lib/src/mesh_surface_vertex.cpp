// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface_vertex.h"
#include "krado/geom_surface.h"

namespace krado {

MeshSurfaceVertex::MeshSurfaceVertex(const GeomSurface & geom_surface, double u, double v) :
    MeshVertexAbstract(geom_surface),
    gsurface_(geom_surface),
    uv_(u, v)
{
    this->phys_pt_ = geom_surface.point(this->uv_);
}

MeshSurfaceVertex::MeshSurfaceVertex(const GeomSurface & geom_surface, UVParam uv) :
    MeshVertexAbstract(geom_surface),
    gsurface_(geom_surface),
    uv_(uv)
{
    this->phys_pt_ = geom_surface.point(this->uv_);
}

const GeomSurface &
MeshSurfaceVertex::geom_surface() const
{
    return this->gsurface_;
}

UVParam
MeshSurfaceVertex::parameter() const
{
    return this->uv_;
}

Point
MeshSurfaceVertex::point() const
{
    return this->phys_pt_;
}

} // namespace krado
