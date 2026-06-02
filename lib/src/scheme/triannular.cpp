// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/triannular.h"
#include "krado/exception.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme/equal.h"
#include "krado/vector.h"
#include "krado/utils.h"
#include "krado/range.h"
#include <numeric>

namespace krado {

static const std::string scheme_name = "triannular";

SchemeTriAnnular::SchemeTriAnnular(Options options) : SchemeAnnular(scheme_name), opts_(options) {}

std::string
SchemeTriAnnular::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("radial_intervals={}", this->opts_.radial_intervals));
    return join(", ", spars);
}

void
SchemeTriAnnular::select_curve_scheme(Ptr<MeshCurve> /* curve */)
{
}

void
SchemeTriAnnular::mesh_surface(Ptr<MeshSurface> mesh_surface)
{
    const auto & gsurf = mesh_surface->geom_surface();
    auto n_radial = this->opts_.radial_intervals;
    if (n_radial < 2)
        throw Exception("Parameter 'radial_intervals' must be at least 2");

    auto loops = get_boundary_loops(mesh_surface);
    if (loops.size() != 2)
        throw Exception("'{}' scheme requires exactly 2 boundary loops, found {}",
                        scheme_name,
                        loops.size());

    // Identify inner and outer loops
    auto ctr_pnt = find_center_point(mesh_surface, loops);

    auto avg_dist = [&](const std::vector<Ptr<MeshVertexAbstract>> & loop) {
        double d = 0;
        for (const auto & v : loop)
            d += v->point().distance(ctr_pnt);
        return d / loop.size();
    };

    auto & inner_loop = (avg_dist(loops[0]) < avg_dist(loops[1])) ? loops[0] : loops[1];
    auto & outer_loop = (avg_dist(loops[0]) < avg_dist(loops[1])) ? loops[1] : loops[0];

    auto N_in = static_cast<int>(inner_loop.size()) - 1;
    auto N_out = static_cast<int>(outer_loop.size()) - 1;

    int expected_N_in = N_out - 6 * n_radial;
    if (N_in != expected_N_in)
        throw Exception("For '{}' scheme, inner loop ({}) must have {} segments (outer has "
                        "{}) with {} radial intervals (decreasing by 6 per ring)",
                        scheme_name,
                        N_in,
                        expected_N_in,
                        N_out,
                        n_radial);

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings(n_radial + 1);
    rings[0] = inner_loop;
    rings[n_radial] = outer_loop;

    auto L_in = get_L(inner_loop);
    auto L_out = get_L(outer_loop);
    auto total_L_in = L_in.back();
    auto total_L_out = L_out.back();

    // Generate intermediate rings
    for (auto k : make_range(1, n_radial)) {
        double alpha_r = static_cast<double>(k) / n_radial;
        int Sk = N_in + 6 * k;

        for (auto i : make_range(Sk)) {
            auto l_rel = static_cast<double>(i) / Sk;
            auto p_in = interpolate_loop(inner_loop, L_in, l_rel * total_L_in);
            auto p_out = interpolate_loop(outer_loop, L_out, l_rel * total_L_out);
            Point p = p_in + (p_out - p_in) * alpha_r;

            auto uv = gsurf.parameter_from_point(p);
            auto v = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv);
            mesh_surface->add_vertex(v);
            rings[k].push_back(v);
        }
        rings[k].push_back(rings[k].front());
    }

    // Create triangles between rings
    for (auto k : make_range(n_radial)) {
        auto & inner = rings[k];
        auto & outer = rings[k + 1];
        auto Sin = static_cast<int>(inner.size()) - 1;
        auto Sout = static_cast<int>(outer.size()) - 1;

        int v = 0; // outer index
        int w = 0; // inner index
        while (v < Sout || w < Sin) {
            if (v < Sout && (w == Sin || (double) v / Sout <= (double) w / Sin)) {
                mesh_surface->add_triangle(ccw_triangle(gsurf, outer[v], outer[v + 1], inner[w]));
                v++;
            }
            else {
                mesh_surface->add_triangle(ccw_triangle(gsurf, outer[v], inner[w + 1], inner[w]));
                w++;
            }
        }
    }
}

} // namespace krado
