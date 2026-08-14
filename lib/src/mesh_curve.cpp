// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_curve.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_element.h"
#include "krado/types.h"
#include "krado/utils.h"
#include "krado/log.h"
#include <array>

namespace krado {

MeshCurve::MeshCurve(ShapeID id, GeomCurve & gcurve, Ptr<MeshVertex> v1, Ptr<MeshVertex> v2) :
    id_(id),
    gcurve_(gcurve),
    too_smoll(false)
{
    this->bnd_vtxs_.push_back(v1);
    this->bnd_vtxs_.push_back(v2);
}

MeshCurve::~MeshCurve() = default;

ShapeID
MeshCurve::id() const
{
    return this->id_;
}

const GeomCurve &
MeshCurve::geom_curve() const
{
    return this->gcurve_;
}

Span<const Ptr<MeshVertex>>
MeshCurve::bounding_vertices() const
{
    return this->bnd_vtxs_;
}

Span<Ptr<MeshVertex>>
MeshCurve::bounding_vertices()
{
    return this->bnd_vtxs_;
}

void
MeshCurve::add_vertex(Ptr<MeshCurveVertex> curve_vertex)
{
    this->curve_vtx_.push_back(curve_vertex);
}

Span<const Ptr<MeshCurveVertex>>
MeshCurve::curve_vertices() const
{
    return this->curve_vtx_;
}

Span<Ptr<MeshCurveVertex>>
MeshCurve::curve_vertices()
{
    return this->curve_vtx_;
}

void
MeshCurve::add_segment(const std::array<Ptr<MeshVertexAbstract>, 2> & seg)
{
    MeshElement line2(ElementType::LINE2, { seg[0], seg[1] });
    this->segs_.emplace_back(line2);
}

Span<const MeshElement>
MeshCurve::segments() const
{
    return this->segs_;
}

bool
MeshCurve::is_mesh_degenerated() const
{
    if (this->too_smoll)
        Log::debug("Degenerated mesh on curve {}: too small", id());
    if (this->bnd_vtxs_[0] && this->bnd_vtxs_[0] == this->bnd_vtxs_[1] &&
        this->curve_vtx_.size() == 0)
        Log::debug("Degenerated mesh on curve {}", id());
    return this->too_smoll || (this->bnd_vtxs_[0] && this->bnd_vtxs_[0] == this->bnd_vtxs_[1] &&
                               this->curve_vtx_.size() == 0);
}

void
MeshCurve::set_mesh_size(double size)
{
    this->gcurve_.mesh_size_ = size;
}

void
MeshCurve::set_too_small(bool value)
{
    this->too_smoll = value;
}

bool
MeshCurve::has_scheme() const
{
    return this->scheme_.get() != nullptr;
}

Scheme1D &
MeshCurve::scheme()
{
    if (this->scheme_ == nullptr)
        throw Exception("No scheme assigned on curve {}", id());
    return *this->scheme_.get();
}

std::vector<Ptr<MeshVertexAbstract>>
get_mesh_curve_vertices(Ptr<MeshCurve> curve)
{
    std::vector<Ptr<MeshVertexAbstract>> vtxs;
    vtxs.reserve(curve->curve_vertices().size() + 2);
    auto bnd_vtxs = curve->bounding_vertices();
    vtxs.emplace_back(bnd_vtxs[0]);
    for (auto cv : curve->curve_vertices()) {
        vtxs.emplace_back(cv);
    }
    vtxs.emplace_back(bnd_vtxs[1]);
    return vtxs;
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::MeshCurve & curve)
{
    stream << "Curve " << curve.id() << ": ";
    auto & gcurve = curve.geom_curve();
    stream << "type=" << gcurve.type() << ", ";
    auto bnd_vtxs = curve.bounding_vertices();
    std::vector<krado::i32> vids;
    vids.reserve(vids.size());
    for (auto v : bnd_vtxs)
        vids.push_back(v->id());
    stream << "vertices=[" << krado::join(", ", vids) << "], ";
    auto [umin, umax] = gcurve.param_range();
    stream << "u=[" << umin << ", " << umax << "], ";
    stream << "length=" << gcurve.length();
    return stream;
}
