// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_curve.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/exception.h"
#include "krado/scheme.h"
#include <array>

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, MeshVertex * v1, MeshVertex * v2) :
    gcurve(gcurve),
    bnd_vtxs({ v1, v2 }),
    meshed(false)
{
    set<int>("marker", 0);
}

const GeomCurve &
MeshCurve::geom_curve() const
{
    return this->gcurve;
}

const std::vector<MeshVertexAbstract *> &
MeshCurve::all_vertices() const
{
    return this->vtxs;
}

const std::vector<MeshVertex *> &
MeshCurve::bounding_vertices() const
{
    return this->bnd_vtxs;
}

void
MeshCurve::add_vertex(MeshVertex * vertex)
{
    this->vtxs.push_back(vertex);
}

void
MeshCurve::add_vertex(MeshCurveVertex * curve_vertex)
{
    this->curve_vtx.push_back(curve_vertex);
    this->vtxs.push_back(curve_vertex);
}

const std::vector<MeshCurveVertex *> &
MeshCurve::curve_vertices() const
{
    return this->curve_vtx;
}

std::vector<MeshCurveVertex *> &
MeshCurve::curve_vertices()
{
    return this->curve_vtx;
}

void
MeshCurve::add_segment(const std::array<int, 2> & seg)
{
    auto line2 = Element::Line2(seg);
    this->segs.emplace_back(line2);
}

const std::vector<Element> &
MeshCurve::segments() const
{
    return this->segs;
}

bool
MeshCurve::is_meshed() const
{
    return this->meshed;
}

void
MeshCurve::set_meshed()
{
    this->meshed = true;
}

} // namespace krado
