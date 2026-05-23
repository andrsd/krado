// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/fan.h"
#include "krado/exception.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/log.h"
#include "krado/utils.h"
#include "krado/element.h"
#include "krado/vector.h"
#include "krado/range.h"
#include "krado/circle.h"
#include "krado/arc_of_circle.h"

namespace krado {

static const std::string scheme_name = "fan";

namespace {

/// Find shared vertex (common to two curves)
Ptr<MeshVertex>
find_shared_vertex(Ptr<MeshCurve> crv1, Ptr<MeshCurve> crv2)
{
    auto bnd1 = crv1->bounding_vertices();
    auto bnd2 = crv2->bounding_vertices();
    if (bnd1[0] == bnd2[0] || bnd1[0] == bnd2[1])
        return bnd1[0];
    else if (bnd1[1] == bnd2[0] || bnd1[1] == bnd2[1])
        return bnd1[1];
    else
        return nullptr;
}

} // namespace

SchemeFan::SchemeFan(Options /*options*/) : Scheme2D(scheme_name) {}

void
SchemeFan::mesh_surface(Ptr<MeshSurface> mesh_surface)
{
    Log::info("Meshing surface {}: scheme='fan'", mesh_surface->id());

    const auto & gsurf = mesh_surface->geom_surface();
    auto curves = mesh_surface->curves();
    if (curves.size() != 3)
        throw Exception("Fan scheme requires exactly 3 curves on surface {}", mesh_surface->id());

    Ptr<MeshCurve> circular_edge = nullptr;
    std::vector<Ptr<MeshCurve>> line_edges;

    for (auto & c : curves) {
        if (c->geom_curve().type() == GeomCurve::CurveType::Circle) {
            if (circular_edge)
                throw Exception("Fan scheme requires exactly one circular curve on surface {}",
                                mesh_surface->id());
            circular_edge = c;
        }
        else if (c->geom_curve().type() == GeomCurve::CurveType::Line) {
            line_edges.push_back(c);
        }
        else {
            throw Exception("Fan scheme only supports Circle and Line curves on surface {}",
                            mesh_surface->id());
        }
    }

    if (!circular_edge || line_edges.size() != 2)
        throw Exception("Fan scheme requires one circular curve and two lines on surface {}",
                        mesh_surface->id());

    if (circular_edge->segments().size() == 0)
        throw Exception("Circular edge must be meshed before surface meshing on surface {}",
                        mesh_surface->id());

    if (line_edges[0]->segments().size() != line_edges[1]->segments().size())
        throw Exception("Straight lines must have equal number of segments ({} <=> {})",
                        line_edges[0]->segments().size(),
                        line_edges[1]->segments().size());
    // TODO: check that element sizes on the straight curves match
    // TODO: check orientation of the straight edges

    // number of segments on the circular edge
    const auto N = circular_edge->segments().size();
    // number of radial segments
    const auto M = line_edges[0]->segments().size();
    if (N < M)
        throw Exception("Number of radial segments ({}) must be larger or equal to the number of "
                        "segments on the circular edge ({}).",
                        M,
                        N);

    auto center_vtx = find_shared_vertex(line_edges[0], line_edges[1]);
    if (center_vtx == nullptr)
        throw Exception("The two lines must share a vertex (the center of the fan) on surface {}",
                        mesh_surface->id());

    auto circular_verts = get_mesh_curve_vertices(circular_edge);
    if (circular_verts.size() != N + 1)
        throw Exception("Unexpected number of vertices on circular edge of surface {}",
                        mesh_surface->id());

    // Determine orientation of the circular edge relative to the lines
    // We want circular_verts[0] to be on line1 and circular_verts[N] to be on line2
    bool swap_lines = false;
    auto l0_bnd = line_edges[0]->bounding_vertices();
    if (l0_bnd[0] == circular_verts.back() || l0_bnd[1] == circular_verts.back()) {
        swap_lines = true;
    }

    Ptr<MeshCurve> line1 = swap_lines ? line_edges[1] : line_edges[0];
    Ptr<MeshCurve> line2 = swap_lines ? line_edges[0] : line_edges[1];

    // Generate rings
    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings;
    rings.resize(M + 1);

    // "ring" zero is the circular edge
    rings[0] = circular_verts;
    // Rings 1 to M-1
    for (auto k : make_range(1, M)) {
        auto s = N - k;
        auto idx = M - k - 1;
        auto start = line1->curve_vertices()[idx];
        auto end = line2->curve_vertices()[idx];
        auto p_start = start->point();
        auto p_end = end->point();

        // build arc for each ring so we can lay out the points on concentric circles
        auto radius = utils::distance(p_start, center_vtx->point());
        auto circ = Circle::create(center_vtx->point(), radius);
        auto arc = ArcOfCircle::create(circ, p_start, p_end);
        auto [t_lo, t_hi] = arc.param_range();

        rings[k].push_back(start);
        for (auto i : make_range(1, s)) {
            auto beta = static_cast<double>(i) / s;
            auto t = t_lo + (t_hi - t_lo) * beta;
            auto p = arc.point(t);
            auto uv = gsurf.parameter_from_point(p);
            auto v = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv);
            mesh_surface->add_vertex(v);
            rings[k].push_back(v);
        }
        rings[k].push_back(end);
    }
    // "ring" M is just the center vertex
    rings[M] = { center_vtx };

    // Helpers for quad splitting
    auto q_gamma =
        [](Ptr<MeshVertexAbstract> a, Ptr<MeshVertexAbstract> b, Ptr<MeshVertexAbstract> c) {
            return Tri3::gamma(a->point(), b->point(), c->point());
        };

    auto evaluate_split = [&](Ptr<MeshVertexAbstract> a,
                              Ptr<MeshVertexAbstract> b,
                              Ptr<MeshVertexAbstract> c,
                              Ptr<MeshVertexAbstract> d,
                              int way) {
        if (way == 0)
            return std::min(q_gamma(a, b, c), q_gamma(a, c, d));
        else
            return std::min(q_gamma(a, b, d), q_gamma(b, c, d));
    };

    auto add_split = [&](Ptr<MeshVertexAbstract> a,
                         Ptr<MeshVertexAbstract> b,
                         Ptr<MeshVertexAbstract> c,
                         Ptr<MeshVertexAbstract> d,
                         int way) {
        if (way == 0) {
            mesh_surface->add_triangle(ccw_triangle(gsurf, a, b, c));
            mesh_surface->add_triangle(ccw_triangle(gsurf, a, c, d));
        }
        else {
            mesh_surface->add_triangle(ccw_triangle(gsurf, a, b, d));
            mesh_surface->add_triangle(ccw_triangle(gsurf, b, c, d));
        }
    };

    // "outer" triangle strips
    for (auto k : make_range(M - 1)) {
        auto & outer = rings[k];
        auto & inner = rings[k + 1];

        // number of segments on the outer arc
        auto s = outer.size() - 1;

        if (s == 1) {
            // This should never happen
            throw Exception("Ring {} has 1 segment", k);
        }
        else if (s == 2) {
            auto v0 = outer[0], v1 = outer[1], v2 = outer[2];
            auto w0 = inner[0], w1 = inner[1];

            mesh_surface->add_triangle(ccw_triangle(gsurf, v0, v1, w0));
            mesh_surface->add_triangle(ccw_triangle(gsurf, v1, w1, w0));
            mesh_surface->add_triangle(ccw_triangle(gsurf, v1, v2, w1));
        }
        else {
            // s >= 3
            double best_q = -1.0;
            int best_w1 = 0, best_w2 = 0;

            for (int w1 = 0; w1 < 2; ++w1) {
                for (int w2 = 0; w2 < 2; ++w2) {
                    double q = std::min(
                        evaluate_split(outer[0], outer[1], inner[1], inner[0], w1),
                        evaluate_split(outer[s - 1], outer[s], inner[s - 1], inner[s - 2], w2));
                    if (q > best_q) {
                        best_q = q;
                        best_w1 = w1;
                        best_w2 = w2;
                    }
                }
            }

            add_split(outer[0], outer[1], inner[1], inner[0], best_w1);

            // Middle triangles
            std::size_t v = 1, w = 1;
            while (v < s - 1 || w < s - 2) {
                if (v < s - 1 && (w == s - 2 || v <= w)) {
                    mesh_surface->add_triangle(
                        ccw_triangle(gsurf, outer[v], outer[v + 1], inner[w]));
                    v++;
                }
                else {
                    mesh_surface->add_triangle(
                        ccw_triangle(gsurf, outer[v], inner[w + 1], inner[w]));
                    w++;
                }
            }

            add_split(outer[s - 1], outer[s], inner[s - 1], inner[s - 2], best_w2);
        }
    }
    // "inner" triangle fan
    {
        auto & outer = rings[M - 1];
        for (auto v : make_range(outer.size() - 1))
            mesh_surface->add_triangle(ccw_triangle(gsurf, outer[v], outer[v + 1], center_vtx));
    }

    Log::info("- generated {} triangles", utils::human_number(mesh_surface->triangles().size()));
}

} // namespace krado
