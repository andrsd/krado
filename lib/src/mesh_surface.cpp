// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme.h"
#include <array>

namespace krado {

MeshSurface::MeshSurface(const GeomSurface & gsurface,
                         const std::vector<MeshCurve *> & mesh_curves) :
    gsurface_(gsurface),
    mesh_curves_(mesh_curves)
{
}

const GeomSurface &
MeshSurface::geom_surface() const
{
    return this->gsurface_;
}

const std::vector<MeshCurve *> &
MeshSurface::curves() const
{
    return this->mesh_curves_;
}

const std::vector<MeshVertexAbstract *> &
MeshSurface::all_vertices() const
{
    return this->vtxs_;
}

const std::vector<MeshSurfaceVertex *> &
MeshSurface::surface_vertices() const
{
    return this->surf_vtxs_;
}

const std::vector<Element> &
MeshSurface::triangles() const
{
    return this->tris_;
}

void
MeshSurface::add_vertex(MeshVertex * vertex)
{
    this->vtxs_.push_back(vertex);
}

void
MeshSurface::add_vertex(MeshCurveVertex * vertex)
{
    this->vtxs_.push_back(vertex);
}

void
MeshSurface::add_vertex(MeshSurfaceVertex * vertex)
{
    this->vtxs_.push_back(vertex);
    this->surf_vtxs_.push_back(vertex);
}

void
MeshSurface::add_triangle(const std::array<std::size_t, 3> & tri)
{
    auto tri3 = Element::Tri3(tri);
    this->tris_.emplace_back(tri3);
}

void
MeshSurface::reserve_mem(std::size_t n_vtxs, std::size_t n_tris)
{
    this->vtxs_.reserve(n_vtxs);
    this->tris_.reserve(n_tris);
}

} // namespace krado
