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

std::vector<std::vector<Ptr<MeshVertexAbstract>>>
create_points(Ptr<MeshSurface> mesh_surface,
              int n_radial,
              const std::vector<Ptr<MeshVertexAbstract>> & inner_loop,
              const std::vector<Ptr<MeshVertexAbstract>> & outer_loop)
{
    const auto & gsurf = mesh_surface->geom_surface();
    const auto N_out = static_cast<int>(outer_loop.size()) - 1;

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings(n_radial + 1);
    rings[0] = inner_loop;
    rings[n_radial] = outer_loop;

    const auto L_in = get_L(inner_loop);
    const auto L_out = get_L(outer_loop);
    const auto total_L_in = L_in.back();
    const auto total_L_out = L_out.back();

    // Generate intermediate rings
    for (auto k : make_range(1, n_radial)) {
        const auto alpha_r = static_cast<double>(k) / n_radial;
        const auto Sk = N_out;

        for (auto i : make_range(Sk)) {
            const auto l_rel = static_cast<double>(i) / Sk;
            const auto p_in = interpolate_loop(inner_loop, L_in, l_rel * total_L_in);
            const auto p_out = interpolate_loop(outer_loop, L_out, l_rel * total_L_out);
            const Point p = p_in + (p_out - p_in) * alpha_r;

            const auto uv = gsurf.parameter_from_point(p);
            const auto v = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv);
            mesh_surface->add_vertex(v);
            rings[k].emplace_back(v);
        }
        rings[k].push_back(rings[k].front());
    }
    return rings;
}

void
create_quadrangles(Ptr<MeshSurface> mesh_surface,
                   const std::vector<std::vector<Ptr<MeshVertexAbstract>>> & rings,
                   int n_radial)
{
    const auto & gsurf = mesh_surface->geom_surface();
    // Create quadrangles between rings
    for (auto k : make_range(n_radial)) {
        const auto & inner = rings[k];
        const auto & outer = rings[k + 1];
        const auto N = static_cast<int>(inner.size()) - 1;

        for (auto i : make_range(N)) {
            mesh_surface->add_quadrangle(
                ccw_quadrangle(gsurf, inner[i], outer[i], outer[i + 1], inner[i + 1]));
        }
    }
}

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
    const auto n_radial = this->opts_.radial_intervals;
    if (n_radial < 1)
        throw Exception("Parameter 'radial_intervals' must be at least 1");

    // check that we have 2 bounding curves that are circular
    auto curves = mesh_surface->curves();
    if (curves.size() != 2)
        throw Exception("Surface {} is not bounded by 2 curves", mesh_surface->id());

    for (int i : { 0, 1 }) {
        if (curves[i]->geom_curve().type() != GeomCurve::CurveType::Circle)
            throw Exception("Curve {} is not circular curve", curves[i]->id());
    }

    std::array<double, 2> radius = { get_radius(curves[0]->geom_curve()),
                                     get_radius(curves[1]->geom_curve()) };
    bool inner = radius[0] < radius[1];

    // Identify inner and outer loops
    const auto & inner_loop =
        inner ? get_mesh_curve_vertices(curves[0]) : get_mesh_curve_vertices(curves[1]);
    const auto & outer_loop =
        inner ? get_mesh_curve_vertices(curves[1]) : get_mesh_curve_vertices(curves[0]);

    const auto N_in = static_cast<int>(inner_loop.size()) - 1;
    const auto N_out = static_cast<int>(outer_loop.size()) - 1;

    if (N_in != N_out)
        throw Exception("For '{}' scheme, inner loop ({}) and outer loop ({}) must have the "
                        "same number of segments",
                        scheme_name,
                        N_in,
                        N_out);

    const auto rings = create_points(mesh_surface, n_radial, inner_loop, outer_loop);
    create_quadrangles(mesh_surface, rings, n_radial);
}

} // namespace krado
