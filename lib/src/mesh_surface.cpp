// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_surface.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/consts.h"
#include "krado/utils.h"
#include <array>
#include <cassert>

namespace krado {

MeshSurface::MeshSurface(ShapeID id,
                         const GeomSurface & gsurface,
                         const std::vector<Ptr<MeshCurve>> & mesh_curves) :
    id_(id),
    gsurface_(gsurface),
    mesh_curves_(mesh_curves)
{
}

MeshSurface::~MeshSurface() = default;

ShapeID
MeshSurface::id() const
{
    return this->id_;
}

const GeomSurface &
MeshSurface::geom_surface() const
{
    return this->gsurface_;
}

Span<const Ptr<MeshCurve>>
MeshSurface::curves() const
{
    return this->mesh_curves_;
}

Span<Ptr<MeshCurve>>
MeshSurface::curves()
{
    return this->mesh_curves_;
}

double
MeshSurface::mesh_size() const
{
    assert(this->mesh_size_.has_value());
    return this->mesh_size_.value();
}

void
MeshSurface::set_mesh_size(double size)
{
    this->mesh_size_ = size;
}

double
MeshSurface::mesh_size_at_param(UVParam /* par */) const
{
    if (this->mesh_size_.has_value())
        return this->mesh_size_.value();

    // TODO: if no surface size set, figure it out from edges and vertices

    return MAX_LC;
}

Span<const Ptr<MeshSurfaceVertex>>
MeshSurface::surface_vertices() const
{
    return this->surf_vtxs_;
}

Span<Ptr<MeshSurfaceVertex>>
MeshSurface::surface_vertices()
{
    return this->surf_vtxs_;
}

Span<const MeshElement>
MeshSurface::triangles() const
{
    return this->tris_;
}

Span<MeshElement>
MeshSurface::triangles()
{
    return this->tris_;
}

Span<const MeshElement>
MeshSurface::quadrangles() const
{
    return this->quads_;
}

Span<MeshElement>
MeshSurface::quadrangles()
{
    return this->quads_;
}

void
MeshSurface::add_vertex(Ptr<MeshSurfaceVertex> vertex)
{
    this->surf_vtxs_.push_back(vertex);
}

void
MeshSurface::add_triangle(const std::array<Ptr<MeshVertexAbstract>, 3> & tri)
{
    MeshElement tri3(ElementType::TRI3, { tri[0], tri[1], tri[2] });
    this->tris_.emplace_back(tri3);
}

void
MeshSurface::add_quadrangle(const std::array<Ptr<MeshVertexAbstract>, 4> & quad)
{
    MeshElement quad4(ElementType::QUAD4, { quad[0], quad[1], quad[2], quad[3] });
    this->quads_.emplace_back(quad4);
}

void
MeshSurface::add_element(MeshElement tri)
{
    if (tri.type() == ElementType::TRI3)
        this->tris_.emplace_back(tri);
    else if (tri.type() == ElementType::QUAD4)
        this->quads_.emplace_back(tri);
    else
        throw Exception("Unsupported element type");
}

void
MeshSurface::quads_to_tris(QuadSplitMode mode)
{
    if (this->quads_.empty())
        return;

    for (const auto & quad : this->quads_) {
        auto v = quad.vertices();
        if (mode == QuadSplitMode::SPLIT2) {
            add_triangle({ v[0], v[1], v[2] });
            add_triangle({ v[2], v[3], v[0] });
        }
        else if (mode == QuadSplitMode::SPLIT4) {
            const auto & gsurf = geom_surface();
            Point center_pt =
                0.25 * (v[0]->point() + v[1]->point() + v[2]->point() + v[3]->point());
            auto uv = gsurf.parameter_from_point(center_pt);

            auto center_vtx = Ptr<MeshSurfaceVertex>::alloc(this->gsurface_, uv);
            add_triangle({ v[0], v[1], center_vtx });
            add_triangle({ v[1], v[2], center_vtx });
            add_triangle({ v[2], v[3], center_vtx });
            add_triangle({ v[3], v[0], center_vtx });
        }
    }
    this->quads_.clear();
}

void
MeshSurface::reserve_mem(std::size_t n_tris)
{
    this->tris_.reserve(n_tris);
}

void
MeshSurface::set_triangles(const std::vector<MeshElement> & new_tris)
{
    this->tris_ = new_tris;
}

Span<const MeshElement>
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
    this->surf_vtxs_.clear();
    this->tris_.clear();
    this->quads_.clear();
}

bool
MeshSurface::has_scheme() const
{
    return this->scheme_.get() != nullptr;
}

Scheme2D &
MeshSurface::scheme()
{
    if (this->scheme_ == nullptr)
        throw Exception("No scheme assigned on surface {}", id());
    return *this->scheme_.get();
}

} // namespace krado

std::ostream &
operator<<(std::ostream & stream, const krado::MeshSurface & srf)
{
    stream << "Surface " << srf.id() << ": ";
    auto crvs = srf.curves();
    std::vector<krado::i32> cids;
    cids.reserve(crvs.size());
    for (auto c : crvs)
        cids.push_back(c->id());
    stream << "curves=[" << krado::join(", ", cids) << "], ";
    auto & gsurf = srf.geom_surface();
    auto [u_min, u_max] = gsurf.param_range(0);
    auto [v_min, v_max] = gsurf.param_range(1);
    stream << "(u, v)=[" << u_min << ", " << u_max << "]x";
    stream << "[" << v_min << ", " << v_max << "], ";
    stream << "area=" << gsurf.area();
    return stream;
}
