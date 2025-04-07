// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_curve.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_element.h"
#include "krado/exception.h"
#include "krado/scheme.h"
#include "krado/types.h"
#include "krado/log.h"
#include <array>

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, MeshVertex * v1, MeshVertex * v2) :
    gcurve_(gcurve),
    bnd_vtxs_({ v1, v2 }),
    too_smoll(false)
{
}

int
MeshCurve::id() const
{
    return this->gcurve_.id();
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

bool
MeshCurve::is_mesh_degenerated() const
{
    if (this->too_smoll)
        Log::debug("Degenerated mesh on curve {}: too small", id());
    if (this->bnd_vtxs_[0] && this->bnd_vtxs_[0] == this->bnd_vtxs_[1] && this->vtxs_.size() < 2)
        Log::debug("Degenerated mesh on curve {}: {} mesh nodes", id(), (int) this->vtxs_.size());
    return this->too_smoll || (this->bnd_vtxs_[0] && this->bnd_vtxs_[0] == this->bnd_vtxs_[1] &&
                               this->vtxs_.size() < 2);
}

void
MeshCurve::set_too_small(bool value)
{
    this->too_smoll = value;
}

//

bool
MEdgeLessThan::operator()(const MeshElement & e1, const MeshElement & e2) const
{
    auto vtx1 = e1.vertices();
    if (vtx1[0]->num() > vtx1[1]->num())
        std::swap(vtx1[0], vtx1[1]);
    auto vtx2 = e2.vertices();
    if (vtx2[0]->num() > vtx2[1]->num())
        std::swap(vtx2[0], vtx2[1]);

    if (vtx1[0]->num() < vtx2[0]->num())
        return true;
    if (vtx1[0]->num() > vtx2[0]->num())
        return false;
    if (vtx1[1]->num() < vtx2[1]->num())
        return true;
    return false;
}

} // namespace krado
