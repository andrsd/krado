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
#include <chrono>

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
sub_connect(Span<const Index> element_connect, const std::vector<u8> & idxs)
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
angle(Point p1, Point p2, Point p3)
{
    auto a = p1 - p2;
    auto b = p3 - p2;
    auto c = cross_product(a, b);
    auto sin_alpha = c.magnitude();
    auto cos_alpha = dot_product(a, b);
    return std::atan2(sin_alpha, cos_alpha);
}

double
distance(Point p1, Point p2)
{
    auto delta = p1 - p2;
    return std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

double
distance(UVParam p1, UVParam p2)
{
    UVParam delta(p1.u - p2.u, p1.v - p2.v);
    return std::sqrt(delta.u * delta.u + delta.v * delta.v);
}

std::string
human_time(double time)
{
    using namespace std::chrono;
    duration<double, std::micro> us(time * 1e6);
    auto h = duration_cast<hours>(us);
    us -= h;
    auto m = duration_cast<minutes>(us);
    us -= m;
    auto s = duration_cast<seconds>(us);
    us -= s;
    auto ms = duration_cast<milliseconds>(us);

    std::vector<std::string> strs;
    if (time > 1.) {
        if (h.count() > 0)
            strs.push_back(fmt::format("{}h", h.count()));
        if (m.count() > 0)
            strs.push_back(fmt::format("{}m", m.count()));
        // Long running: Show seconds (include ms as decimals, e.g., 5.23s)
        if ((s.count() > 0) || (h.count() == 0 && m.count() == 0)) {
            double total_seconds = s.count() + (ms.count() / 1000.0);
            strs.push_back(fmt::format("{:.2f}s", total_seconds));
        }
    }
    else {
        // Short running (< 1 second): Show pure milliseconds (e.g., 23.45ms)
        double total_ms = us.count() / 1000.0;
        strs.push_back(fmt::format("{:.2f}ms", total_ms));
    }
    return join(" ", strs);
}

[[nodiscard]]
std::vector<Index>
get_face_connect(const Element & elem, int side)
{
    std::vector<Index> face_connect;
    if (elem.type() == ElementType::TETRA4)
        return utils::sub_connect(elem.indices(), Tetra4::FACE_VERTICES[side]);
    else if (elem.type() == ElementType::PYRAMID5)
        return utils::sub_connect(elem.indices(), Pyramid5::FACE_VERTICES[side]);
    else if (elem.type() == ElementType::PRISM6)
        return utils::sub_connect(elem.indices(), Prism6::FACE_VERTICES[side]);
    else if (elem.type() == ElementType::HEX8)
        return utils::sub_connect(elem.indices(), Hex8::FACE_VERTICES[side]);
    else
        throw Exception("Unsupported element type {}", elem.type());
}

} // namespace utils

std::vector<SideEntry>
create_side_set(const Mesh & mesh, const std::vector<Index> & idxs)
{
    std::vector<SideEntry> sset;
    sset.reserve(idxs.size());
    for (auto & f : idxs) {
        auto support = mesh.support(f);
        if (support.size() != 1)
            throw Exception("Facet {} is not a boundary facet", f);

        auto cell = support[0];
        auto cell_connect = mesh.cone(cell);
        auto lfi = utils::index_of(cell_connect, f);
        if (lfi.has_value())
            sset.emplace_back(cell, lfi.value());
    }
    return sset;
}

std::vector<SideEntry>
create_side_set(Ptr<const Mesh> mesh, const std::vector<Index> & idxs)
{
    if (mesh)
        return create_side_set(*mesh, idxs);
    else
        throw Exception("Null pointer access");
}

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
