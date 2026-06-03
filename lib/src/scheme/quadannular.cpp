// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/quadannular.h"
#include "krado/exception.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/vector.h"
#include "krado/utils.h"
#include "krado/range.h"

namespace krado {

namespace {

const std::string scheme_name = "quadannular";

} // namespace

SchemeQuadAnnular::SchemeQuadAnnular(Options options) : SchemeAnnular(scheme_name), opts_(options)
{
}

std::string
SchemeQuadAnnular::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("radial_intervals={}", this->opts_.radial_intervals));
    return join(", ", spars);
}

void
SchemeQuadAnnular::select_curve_scheme(Ptr<MeshCurve> /* curve */)
{
}

void
SchemeQuadAnnular::mesh_surface(Ptr<MeshSurface> mesh_surface)
{
    const auto & gsurf = mesh_surface->geom_surface();
    auto n_radial = this->opts_.radial_intervals;
    if (n_radial < 1)
        throw Exception("Parameter 'radial_intervals' must be at least 1");

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

    if (N_in != N_out)
        throw Exception("For '{}' scheme, inner loop ({}) and outer loop ({}) must have the "
                        "same number of segments",
                        scheme_name,
                        N_in,
                        N_out);

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings(n_radial + 1);
    rings[0] = inner_loop;
    rings[n_radial] = outer_loop;

    auto L_in = get_L(inner_loop);
    auto L_out = get_L(outer_loop);
    auto total_L_in = L_in.back();
    auto total_L_out = L_out.back();

    // Generate intermediate rings
    for (auto k : make_range(1, n_radial)) {
        auto alpha_r = static_cast<double>(k) / n_radial;
        auto Sk = N_out;

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

    // Create quadrangles between rings
    for (auto k : make_range(n_radial)) {
        auto & inner = rings[k];
        auto & outer = rings[k + 1];
        auto N = static_cast<int>(inner.size()) - 1;

        for (auto i : make_range(N)) {
            mesh_surface->add_quadrangle(
                ccw_quadrangle(gsurf, inner[i], outer[i], outer[i + 1], inner[i + 1]));
        }
    }
}

} // namespace krado
