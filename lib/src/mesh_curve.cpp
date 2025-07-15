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
#include "krado/consts.h"
#include <array>

namespace krado {

MeshCurve::MeshCurve(const GeomCurve & gcurve, MeshVertex * v1, MeshVertex * v2) :
    gcurve_(gcurve),
    too_smoll(false)
{
    this->bnd_vtxs_.push_back(v1);
    if (v1 && v2) {
        if (utils::distance(v1->point(), v2->point()) > 1e-12)
            this->bnd_vtxs_.push_back(v2);
    }
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

double
MeshCurve::mesh_size() const
{
    assert(this->mesh_size_.has_value());
    return this->mesh_size_.value();
}

void
MeshCurve::set_mesh_size(double size)
{
    this->mesh_size_ = size;
}

void
MeshCurve::set_too_small(bool value)
{
    this->too_smoll = value;
}

double
MeshCurve::mesh_size_at_param(double u) const
{
    auto [u_lo, u_hi] = this->gcurve_.param_range();
    if (this->mesh_size_.has_value()) {
        return this->mesh_size_.value();
    }
    else if (this->bnd_vtxs_[0] && this->bnd_vtxs_[1]) {
        // 2 bounding vertices => interpolate the size
        double lc1 = this->bnd_vtxs_[0]->mesh_size();
        double lc2 = this->bnd_vtxs_[1]->mesh_size();
        auto alpha = (u - u_lo) / (u_hi - u_lo);
        return (1 - alpha) * lc1 + alpha * lc2;
    }
    else if (this->bnd_vtxs_[0] && std::abs(u - u_lo) < EPSILON) {
        return this->bnd_vtxs_[0]->mesh_size();
    }
    else if (this->bnd_vtxs_[1] && std::abs(u - u_hi) < EPSILON) {
        return this->bnd_vtxs_[1]->mesh_size();
    }
    else
        return MAX_LC;
}

} // namespace krado
