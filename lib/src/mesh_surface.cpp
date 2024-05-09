// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"

namespace krado {

MeshSurface::MeshSurface(const GeomSurface & gsurface,
                         const std::vector<MeshCurve *> & mesh_curves) :
    gsurface(gsurface),
    mesh_curves(mesh_curves),
    meshed(false)
{
    auto & mpars = meshing_parameters();
    mpars.set<std::string>("scheme") = "auto";
    mpars.set<int>("marker") = 0;
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

const std::vector<MeshElement> &
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
}

void
MeshSurface::add_triangle(int idx1, int idx2, int idx3)
{
    auto tri3 = MeshElement::Tri3(idx1, idx2, idx3);
    this->tris.emplace_back(tri3);
}

bool
MeshSurface::is_meshed() const
{
    return this->meshed;
}

void
MeshSurface::set_meshed()
{
    this->meshed = true;
}

} // namespace krado
