// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/utils.h"
#include "krado/point.h"
#include "krado/types.h"
#include "krado/exception.h"
#include "krado/uv_param.h"
#include "krado/vector.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh.h"
#include "boost/functional/hash.hpp"
#include "krado/geom_surface.h"
#include "krado/predicates.h"
#include "krado/mesh_curve.h"
#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include <cstdint>

namespace krado {
namespace utils {

std::string
to_upper(const std::string & text)
{
    std::string upper(text);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

std::string
to_lower(const std::string & text)
{
    std::string lower(text);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

std::vector<Index>
sub_connect(const std::vector<Index> & element_connect, const std::vector<int> & idxs)
{
    std::vector<Index> connect;
    for (auto i : idxs)
        connect.emplace_back(element_connect[i]);
    return connect;
}

std::size_t
key(const std::vector<Index> & idxs)
{
    std::vector<Index> vertices(idxs.begin(), idxs.end());
    std::sort(vertices.begin(), vertices.end());

    std::size_t hash_value = 0;
    for (auto v : vertices)
        boost::hash_combine(hash_value, v);

    return hash_value;
}

template <>
std::string
to_str(ElementType val)
{
    if (val == ElementType::POINT)
        return "POINT";
    else if (val == ElementType::LINE2)
        return "LINE2";
    else if (val == ElementType::TRI3)
        return "TRI3";
    else if (val == ElementType::QUAD4)
        return "QUAD4";
    else if (val == ElementType::TETRA4)
        return "TETRA4";
    else if (val == ElementType::PYRAMID5)
        return "PYRAMID5";
    else if (val == ElementType::PRISM6)
        return "PRISM6";
    else if (val == ElementType::HEX8)
        return "HEX8";
    else
        return "UNKNOWN";
}

double
angle(const Point & p1, const Point & p2, const Point & p3)
{
    auto a = p1 - p2;
    auto b = p3 - p2;
    auto c = cross_product(a, b);
    auto sin_alpha = c.magnitude();
    auto cos_alpha = dot_product(a, b);
    return std::atan2(sin_alpha, cos_alpha);
}

double
distance(const Point & p1, const Point & p2)
{
    auto delta = p1 - p2;
    return std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

double
distance(const UVParam & p1, const UVParam & p2)
{
    UVParam delta(p1.u - p2.u, p1.v - p2.v);
    return std::sqrt(delta.u * delta.u + delta.v * delta.v);
}

std::vector<SideEntry>
create_side_set(const Mesh & mesh, const std::vector<Index> & facets, std::size_t ofst)
{
    std::vector<SideEntry> sset;
    sset.reserve(facets.size());
    for (auto & f : facets) {
        auto support = mesh.support(f);
        if (support.size() != 1)
            throw Exception("Facet {} is not a boundary facet", f);

        auto cell = support[0];
        auto cell_connect = mesh.cone(cell);
        auto lfi = utils::index_of(cell_connect, f);
        sset.emplace_back(cell + ofst, lfi);
    }
    return sset;
}

std::vector<Index>
set_from_side_set(const Mesh & mesh, const std::vector<SideEntry> & side_set)
{
    std::vector<Index> sset;
    sset.reserve(side_set.size());
    for (auto & ent : side_set) {
        auto cell_connect = mesh.cone(ent.elem);
        auto facet = cell_connect[ent.side];
        sset.push_back(facet);
    }
    return sset;
}

void
build_curve_segments(Ptr<MeshCurve> curve)
{
    auto & geom_curve = curve->geom_curve();
    auto bnd_verts = curve->bounding_vertices();
    if ((geom_curve.type() == GeomCurve::CurveType::Circle) && (bnd_verts.size() == 1)) {
        std::vector<Ptr<MeshVertexAbstract>> all;
        // curve is a full circle
        all.push_back(static_ptr_cast<MeshVertexAbstract>(bnd_verts[0]));
        for (auto & cv : curve->curve_vertices())
            all.push_back(static_ptr_cast<MeshVertexAbstract>(cv));

        for (std::size_t i = 0; i + 1 < all.size(); ++i)
            curve->add_segment({ all[i], all[i + 1] });
        curve->add_segment({ all.back(), static_ptr_cast<MeshVertexAbstract>(bnd_verts[0]) });
    }
    else {
        if (bnd_verts.size() != 2)
            throw Exception("Curve {} must have 2 bounding vertices", curve->id());

        std::vector<Ptr<MeshVertexAbstract>> all;
        all.push_back(static_ptr_cast<MeshVertexAbstract>(bnd_verts[0]));
        for (auto & cv : curve->curve_vertices())
            all.push_back(static_ptr_cast<MeshVertexAbstract>(cv));
        all.push_back(static_ptr_cast<MeshVertexAbstract>(bnd_verts[1]));

        for (std::size_t i = 0; i + 1 < all.size(); ++i)
            curve->add_segment({ all[i], all[i + 1] });
    }
}

} // namespace utils

std::array<Ptr<MeshVertexAbstract>, 3>
ccw_triangle(const GeomSurface & gsurf,
             Ptr<MeshVertexAbstract> a,
             Ptr<MeshVertexAbstract> b,
             Ptr<MeshVertexAbstract> c)
{
    auto uv_a = gsurf.parameter_from_point(a->point());
    auto uv_b = gsurf.parameter_from_point(b->point());
    auto uv_c = gsurf.parameter_from_point(c->point());

    auto orientation = orient2d(uv_a, uv_b, uv_c);
    if (orientation > 0)
        return std::array<Ptr<MeshVertexAbstract>, 3> { a, b, c };
    else if (orientation < 0)
        return std::array<Ptr<MeshVertexAbstract>, 3> { a, c, b };
    else
        throw Exception("Degenerate triangle detected. Points are collinear.");
}

std::array<Ptr<MeshVertexAbstract>, 4>
ccw_quadrangle(const GeomSurface & gsurf,
               Ptr<MeshVertexAbstract> a,
               Ptr<MeshVertexAbstract> b,
               Ptr<MeshVertexAbstract> c,
               Ptr<MeshVertexAbstract> d)
{
    auto uv_a = gsurf.parameter_from_point(a->point());
    auto uv_b = gsurf.parameter_from_point(b->point());
    auto uv_c = gsurf.parameter_from_point(c->point());

    auto orientation = orient2d(uv_a, uv_b, uv_c);
    if (orientation > 0)
        return std::array<Ptr<MeshVertexAbstract>, 4> { a, b, c, d };
    else if (orientation < 0)
        return std::array<Ptr<MeshVertexAbstract>, 4> { a, d, c, b };
    else
        throw Exception("Degenerate quadrangle detected.");
}

} // namespace krado
