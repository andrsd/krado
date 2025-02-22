// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_curve.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/exception.h"
#include "krado/scheme.h"
#include "krado/types.h"
#include <array>

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, MeshVertex * v1, MeshVertex * v2) :
    gcurve_(gcurve),
    bnd_vtxs_({ v1, v2 })
{
}

const GeomCurve &
MeshCurve::geom_curve() const
{
    return this->gcurve_;
}

const std::vector<MeshVertexAbstract *> &
MeshCurve::all_vertices() const
{
    return this->vtxs_;
}

const std::vector<MeshVertex *> &
MeshCurve::bounding_vertices() const
{
    return this->bnd_vtxs_;
}

void
MeshCurve::add_vertex(MeshVertex * vertex)
{
    this->vtxs_.push_back(vertex);
}

void
MeshCurve::add_vertex(MeshCurveVertex * curve_vertex)
{
    this->curve_vtx_.push_back(curve_vertex);
    this->vtxs_.push_back(curve_vertex);
}

const std::vector<MeshCurveVertex *> &
MeshCurve::curve_vertices() const
{
    return this->curve_vtx_;
}

std::vector<MeshCurveVertex *> &
MeshCurve::curve_vertices()
{
    return this->curve_vtx_;
}

void
MeshCurve::add_segment(const std::array<MeshVertexAbstract *, 2> & seg)
{
    MeshElement line2(ElementType::LINE2, { seg[0], seg[1] });
    this->segs_.emplace_back(line2);
}

const std::vector<MeshElement> &
MeshCurve::segments() const
{
    return this->segs_;
}

} // namespace krado
