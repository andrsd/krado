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
    gsurface(gsurface),
    mesh_curves(mesh_curves)
{
}

const GeomSurface &
MeshSurface::geom_surface() const
{
    return this->gsurface;
}

const std::vector<MeshCurve *> &
MeshSurface::curves() const
{
    return this->mesh_curves;
}

const std::vector<MeshVertexAbstract *> &
MeshSurface::all_vertices() const
{
    return this->vtxs;
}

const std::vector<MeshSurfaceVertex *> &
MeshSurface::surface_vertices() const
{
    return this->surf_vtxs;
}

const std::vector<Element> &
MeshSurface::triangles() const
{
    return this->tris;
}

void
MeshSurface::add_vertex(MeshVertex * vertex)
{
    this->vtxs.push_back(vertex);
}

void
MeshSurface::add_vertex(MeshCurveVertex * vertex)
{
    this->vtxs.push_back(vertex);
}

void
MeshSurface::add_vertex(MeshSurfaceVertex * vertex)
{
    this->vtxs.push_back(vertex);
    this->surf_vtxs.push_back(vertex);
}

void
MeshSurface::add_triangle(const std::array<std::size_t, 3> & tri)
{
    auto tri3 = Element::Tri3(tri);
    this->tris.emplace_back(tri3);
}

} // namespace krado
