// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme.h"
#include <array>
#include <cassert>

namespace krado {

MeshSurface::MeshSurface(const GeomSurface & gsurface,
                         const std::vector<MeshCurve *> & mesh_curves) :
    gsurface_(gsurface),
    mesh_curves_(mesh_curves)
{
}

int
MeshSurface::id() const
{
    return this->gsurface_.id();
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

std::vector<MeshVertexAbstract *> &
MeshSurface::all_vertices()
{
    return this->vtxs_;
}

const std::vector<MeshSurfaceVertex *> &
MeshSurface::surface_vertices() const
{
    return this->surf_vtxs_;
}

std::vector<MeshSurfaceVertex *> &
MeshSurface::surface_vertices()
{
    return this->surf_vtxs_;
}

const std::vector<MeshElement> &
MeshSurface::triangles() const
{
    return this->tris_;
}

std::vector<MeshElement> &
MeshSurface::triangles()
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
MeshSurface::add_triangle(const std::array<MeshVertexAbstract *, 3> & tri)
{
    MeshElement tri3(ElementType::TRI3, { tri[0], tri[1], tri[2] });
    this->tris_.emplace_back(tri3);
}

void
MeshSurface::add_quadrangle(const std::array<MeshVertexAbstract *, 4> & quad)
{
    MeshElement quad4(ElementType::QUAD4, { quad[0], quad[1], quad[2], quad[3] });
    this->quads_.emplace_back(quad4);
}

void
MeshSurface::add_element(MeshElement tri)
{
    if (tri.type() == ElementType::TRI3)
        this->tris_.emplace_back(tri);
    else
        throw Exception("Unsupported element type");
}

void
MeshSurface::reserve_mem(std::size_t n_vtxs, std::size_t n_tris)
{
    this->vtxs_.reserve(n_vtxs);
    this->tris_.reserve(n_tris);
}

void
MeshSurface::set_triangles(const std::vector<MeshElement> & new_tris)
{
    this->tris_ = new_tris;
}

const std::vector<MeshElement> &
MeshSurface::elements() const
{
    return this->tris_;
}

void
MeshSurface::remove_all_triangles()
{
    this->tris_.clear();
}

void
MeshSurface::delete_mesh()
{
    // this->vtxs_.clear();
    this->surf_vtxs_.clear();
    this->tris_.clear();
}

} // namespace krado
