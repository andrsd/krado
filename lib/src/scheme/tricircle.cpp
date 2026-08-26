// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/tricircle.h"
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
#include <vector>

namespace krado {
namespace {

std::vector<Ptr<MeshVertexAbstract>>
circumference_vertices(const Ptr<MeshSurface> mesh_surface)
{
    std::vector<Ptr<MeshVertexAbstract>> circum_verts;
    for (auto & mesh_crv : mesh_surface->curves()) {
        const auto cvs = get_mesh_curve_vertices(mesh_crv);
        if (circum_verts.empty()) {
            circum_verts = cvs;
        }
        else {
            if (circum_verts.back() == cvs.front())
                circum_verts.insert(circum_verts.end(), cvs.begin() + 1, cvs.end());
            else
                circum_verts.insert(circum_verts.end(), cvs.begin(), cvs.end());
        }
    }
    // Ensure it's closed
    if (circum_verts.front() != circum_verts.back())
        circum_verts.push_back(circum_verts.front());
    return circum_verts;
}

std::vector<std::vector<Ptr<MeshVertexAbstract>>>
create_points(Ptr<MeshSurface> mesh_surface,
              Point ctr_pnt,
              const std::vector<Ptr<MeshVertexAbstract>> & circum_verts,
              int n_radial,
              int S1)
{
    const auto & gsurf = mesh_surface->geom_surface();

    const auto N = static_cast<int>(circum_verts.size()) - 1;

    const auto uv_ctr = gsurf.parameter_from_point(ctr_pnt);
    const auto ctr = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv_ctr);
    mesh_surface->add_vertex(ctr);

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings(n_radial + 1);
    rings[n_radial] = circum_verts;

    // Cumulative distance along the boundary for interpolation
    std::vector<double> L(N + 1, 0.0);
    for (int i = 1; i <= N; ++i) {
        L[i] = L[i - 1] + utils::distance(circum_verts[i - 1]->point(), circum_verts[i]->point());
    }
    const double total_L = L[N];

    // Generate intermediate rings
    for (int k = n_radial - 1; k >= 1; --k) {
        // Linearly interpolate the number of segments between S1 and N
        // Sk = S1 + (N - S1) * (k-1) / (n_radial - 1)
        int Sk;
        if (n_radial > 1) {
            double alpha_s = static_cast<double>(k - 1) / (n_radial - 1);
            Sk = S1 + static_cast<int>(std::round((N - S1) * alpha_s));
        }
        else {
            Sk = S1;
        }

        const double alpha_r = static_cast<double>(k) / static_cast<double>(n_radial);

        for (int i = 0; i < Sk; ++i) {
            const double l = total_L * (static_cast<double>(i) / Sk);

            // Find j such that L[j] <= l < L[j+1]
            const auto it = std::lower_bound(L.begin(), L.end(), l);
            int j = std::distance(L.begin(), it);
            if (j > 0)
                j--;
            if (j >= N)
                j = N - 1;

            const double beta = (total_L > 0) ? (l - L[j]) / (L[j + 1] - L[j]) : 0.0;
            const auto p_bnd = circum_verts[j]->point() +
                               (circum_verts[j + 1]->point() - circum_verts[j]->point()) * beta;

            const auto p = ctr_pnt + (p_bnd - ctr_pnt) * alpha_r;
            const auto uv = gsurf.parameter_from_point(p);
            const auto v = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv);
            mesh_surface->add_vertex(v);
            rings[k].emplace_back(v);
        }
        // Close the ring
        rings[k].push_back(rings[k].front());
    }
    // ring 0 is center
    rings[0] = { ctr };

    return rings;
}

void
create_triangles(Ptr<MeshSurface> mesh_surface,
                 const std::vector<std::vector<Ptr<MeshVertexAbstract>>> & rings,
                 int n_radial)
{
    const auto & gsurf = mesh_surface->geom_surface();
    // ring 0 is center
    const auto ctr = rings[0][0];

    // center fan (ring 0 -> ring 1)
    const auto & r1 = rings[1];
    for (size_t i = 0; i < r1.size() - 1; ++i) {
        mesh_surface->add_triangle(ccw_triangle(gsurf, ctr, r1[i], r1[i + 1]));
    }

    // ring-to-ring strips
    for (int k = 1; k < n_radial; ++k) {
        const auto & inner = rings[k];
        const auto & outer = rings[k + 1];
        const auto Sin = static_cast<int>(inner.size()) - 1;
        const auto Sout = static_cast<int>(outer.size()) - 1;

        int v = 0; // outer index
        int w = 0; // inner index
        while (v < Sout || w < Sin) {
            if (v < Sout &&
                (w == Sin || static_cast<double>(v) / Sout <= static_cast<double>(w) / Sin)) {
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

} // namespace

static const std::string scheme_name = "tricircle";

SchemeTriCircle::SchemeTriCircle(Options options) : Scheme(scheme_name), Scheme2D(), opts_(options)
{
}

std::string
SchemeTriCircle::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("radial_intervals={}", this->opts_.radial_intervals));
    spars.push_back(fmt::format("symmetry={}",
                                this->opts_.symmetry_type == SymmetryType::QUADRANT ? "quadrant"
                                                                                    : "hexagonal"));
    return join(", ", spars);
}

void
SchemeTriCircle::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        // We use a step of 6 for both symmetry types to grow the ring sizes faster,
        // which helps maintain better triangle quality (closer to equilateral).
        const int S1 = (this->opts_.symmetry_type == SymmetryType::HEXAGONAL) ? 6 : 4;
        const int step = 6;
        const int n_intervals = S1 + step * (this->opts_.radial_intervals - 1);
        SchemeEqual::Options opts;
        opts.intervals = n_intervals;
        curve->set_scheme<SchemeEqual>(opts);
    }
}

void
SchemeTriCircle::mesh_surface(Ptr<MeshSurface> mesh_surface)
{
    const auto & gsurf = mesh_surface->geom_surface();
    if (!is_circular_face(gsurf))
        throw Exception("Surface {} is not a circle", mesh_surface->id());

    const auto n_radial = this->opts_.radial_intervals;
    if (n_radial <= 0)
        throw Exception("Parameter 'radial_intervals' must be a positive number");

    // Collect all boundary vertices in order
    const auto circum_verts = circumference_vertices(mesh_surface);

    // Target S1 (segments in the innermost ring)
    const int S1 = (this->opts_.symmetry_type == SymmetryType::HEXAGONAL) ? 6 : 4;
    // N is number of segments on the boundary
    const auto N = static_cast<int>(circum_verts.size()) - 1;
    if (N < S1)
        throw Exception("Boundary must have at least {} segments for the selected symmetry.", S1);

    // Center vertex
    const auto geom_crv = gsurf.curves()[0];
    const auto ctr_pnt = get_circle_center(geom_crv);

    const auto rings = create_points(mesh_surface, ctr_pnt, circum_verts, n_radial, S1);
    create_triangles(mesh_surface, rings, n_radial);
}

} // namespace krado
