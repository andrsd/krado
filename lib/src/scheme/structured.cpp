// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/structured.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_curve.h"
#include "krado/geom_vertex.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/exception.h"
#include "krado/log.h"
#include "krado/utils.h"
#include <vector>
#include <algorithm>

namespace krado {

namespace {

std::vector<Ptr<MeshVertexAbstract>>
get_ordered_vertices(Ptr<MeshCurve> curve)
{
    // The orientation of the GeomCurve dictates if we need to reverse the order of vertices.
    std::vector<Ptr<MeshVertexAbstract>> v;
    auto bnd_vtxs = curve->bounding_vertices();
    v.push_back(bnd_vtxs[0]);
    for (auto & cv : curve->curve_vertices())
        v.push_back(cv);
    if (bnd_vtxs.size() > 1)
        v.push_back(bnd_vtxs[1]);

    if (curve->geom_curve().orientation() == GeomCurve::Orientation::Reversed) {
        std::reverse(v.begin(), v.end());
    }
    return v;
}

// Sort curves into a closed loop.
Optional<std::vector<Ptr<MeshCurve>>>
sort_curves(std::vector<Ptr<MeshCurve>> curves)
{
    // We'll use the bounding vertices of the GeomCurve to match them.
    std::vector<Ptr<MeshCurve>> sorted_curves;
    std::vector<Ptr<MeshCurve>> remaining_curves = curves;

    sorted_curves.push_back(remaining_curves.back());
    remaining_curves.pop_back();

    while (!remaining_curves.empty()) {
        auto last_curve = sorted_curves.back();
        auto last_vtx = last_curve->geom_curve().last_vertex().point();

        bool found = false;
        for (auto it = remaining_curves.begin(); it != remaining_curves.end(); ++it) {
            auto first = (*it)->geom_curve().first_vertex().point();
            auto last = (*it)->geom_curve().last_vertex().point();

            if (last_vtx.distance(first) < 1e-7 || last_vtx.distance(last) < 1e-7) {
                sorted_curves.push_back(*it);
                remaining_curves.erase(it);
                found = true;
                break;
            }
        }
        if (!found)
            return std::nullopt;
    }

    return sorted_curves;
}

} // namespace

SchemeStructured::SchemeStructured(Options options) : Scheme2D("structured"), opts_(options) {}

void
SchemeStructured::mesh_surface(Ptr<MeshSurface> surface)
{
    Log::info("Meshing surface {}: scheme='structured'", surface->id());

    auto curves = surface->curves();
    if (curves.size() != 4)
        throw Exception("Scheme 'structured' only meshes geometries with 4 curves");

    for (auto & c : curves) {
        if (c->geom_curve().type() != GeomCurve::CurveType::Line)
            throw Exception(
                "Scheme 'structured' only meshes geometries where all curves are lines");
    }

    // Check if curves are meshed
    for (auto & c : curves) {
        if (c->segments().empty())
            throw Exception("Scheme 'structured' requires all curves to be meshed first");
    }

    auto res = sort_curves(curves);
    if (not res.has_value())
        throw Exception("Curves in surface {} do not form a closed loop", surface->id());

    // Now sorted_curves should be in a loop. 0 and 2 should be opposite, 1 and 3 should be
    // opposite.
    auto sorted_curves = res.value();

    auto v0 = get_ordered_vertices(sorted_curves[0]);
    auto v1 = get_ordered_vertices(sorted_curves[1]);
    auto v2 = get_ordered_vertices(sorted_curves[2]);
    auto v3 = get_ordered_vertices(sorted_curves[3]);

    // After ordering, v0 ends at the start of v1, v1 ends at the start of v2, etc.
    // But we also need to ensure they follow each other in the loop.
    auto ensure_loop_order = [](std::vector<Ptr<MeshVertexAbstract>> & v_curr,
                                const std::vector<Ptr<MeshVertexAbstract>> & v_prev) {
        if (v_curr.front() == v_prev.back()) {
            // Correct order
        }
        else if (v_curr.back() == v_prev.back()) {
            std::reverse(v_curr.begin(), v_curr.end());
        }
        else {
            // This might happen if they are not matching at all, but sorting should have handled it
        }
    };

    ensure_loop_order(v1, v0);
    ensure_loop_order(v2, v1);
    ensure_loop_order(v3, v2);

    if (v0.size() != v2.size())
        throw Exception("Opposite curves 0 and 2 must have the same number of segments for scheme "
                        "'structured'");
    if (v1.size() != v3.size())
        throw Exception("Opposite curves 1 and 3 must have the same number of segments for scheme "
                        "'structured'");

    auto ni = v0.size();
    auto nj = v1.size();

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> grid(
        ni,
        std::vector<Ptr<MeshVertexAbstract>>(nj));

    // v0: i=0..ni-1, j=0
    // v2: i=ni-1..0, j=nj-1 (reversed because it's the other side of the loop)
    // v1: i=ni-1, j=0..nj-1
    // v3: i=0, j=nj-1..0

    for (std::size_t i = 0; i < ni; ++i)
        grid[i][0] = v0[i];
    for (std::size_t j = 0; j < nj; ++j)
        grid[ni - 1][j] = v1[j];
    for (std::size_t i = 0; i < ni; ++i)
        grid[ni - 1 - i][nj - 1] = v2[i];
    for (std::size_t j = 0; j < nj; ++j)
        grid[0][nj - 1 - j] = v3[j];

    auto & gsurf = surface->geom_surface();
    for (std::size_t i = 1; i < ni - 1; ++i) {
        for (std::size_t j = 1; j < nj - 1; ++j) {
            auto uv00 = gsurf.parameter_from_point(grid[0][0]->point());
            auto uv10 = gsurf.parameter_from_point(grid[ni - 1][0]->point());
            auto uv01 = gsurf.parameter_from_point(grid[0][nj - 1]->point());
            auto uv11 = gsurf.parameter_from_point(grid[ni - 1][nj - 1]->point());

            auto ri = (double) i / (ni - 1);
            auto rj = (double) j / (nj - 1);

            auto uv_i0 = gsurf.parameter_from_point(grid[i][0]->point());
            auto uv_i1 = gsurf.parameter_from_point(grid[i][nj - 1]->point());
            auto uv_0j = gsurf.parameter_from_point(grid[0][j]->point());
            auto uv_1j = gsurf.parameter_from_point(grid[ni - 1][j]->point());

            auto u = (1 - rj) * uv_i0.u + rj * uv_i1.u + (1 - ri) * uv_0j.u + ri * uv_1j.u -
                     ((1 - ri) * (1 - rj) * uv00.u + ri * (1 - rj) * uv10.u +
                      (1 - ri) * rj * uv01.u + ri * rj * uv11.u);
            auto v = (1 - rj) * uv_i0.v + rj * uv_i1.v + (1 - ri) * uv_0j.v + ri * uv_1j.v -
                     ((1 - ri) * (1 - rj) * uv00.v + ri * (1 - rj) * uv10.v +
                      (1 - ri) * rj * uv01.v + ri * rj * uv11.v);

            auto vtx = Ptr<MeshSurfaceVertex>::alloc(gsurf, u, v);
            surface->add_vertex(vtx);
            grid[i][j] = static_ptr_cast<MeshVertexAbstract>(vtx);
        }
    }

    for (std::size_t i = 0; i < ni - 1; ++i) {
        for (std::size_t j = 0; j < nj - 1; ++j) {
            auto q = ccw_quadrangle(gsurf,
                                    grid[i][j],
                                    grid[i + 1][j],
                                    grid[i + 1][j + 1],
                                    grid[i][j + 1]);
            surface->add_quadrangle(q);
        }
    }
}

} // namespace krado
