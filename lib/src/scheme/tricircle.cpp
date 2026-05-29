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
#include "krado/range.h"

namespace krado {

static const std::string scheme_name = "tricircle";

SchemeTriCircle::SchemeTriCircle(Options options) : Scheme(scheme_name), Scheme2D(), opts_(options)
{
}

std::string
SchemeTriCircle::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("radial_intervals={}", this->opts_.radial_intervals));
    return join(", ", spars);
}

void
SchemeTriCircle::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        // To maintain symmetry and the 4-triangle center constraint,
        // we need N to be a multiple of 4 and N >= 4 * radial_intervals.
        int n_intervals = 4 * this->opts_.radial_intervals;
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

    auto n_radial = this->opts_.radial_intervals;
    if (n_radial <= 0)
        throw Exception("Parameter 'radial_intervals' must be a positive number");

    // Center vertex
    auto geom_crv = gsurf.curves()[0];
    auto ctr_pnt = get_circle_center(geom_crv);
    auto uv_ctr = gsurf.parameter_from_point(ctr_pnt);
    auto ctr = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv_ctr);
    mesh_surface->add_vertex(ctr);

    // Collect all boundary vertices in order
    std::vector<Ptr<MeshVertexAbstract>> circum_verts;
    for (auto & mesh_crv : mesh_surface->curves()) {
        auto cvs = get_mesh_curve_vertices(mesh_crv);
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

    // N is number of segments on the boundary
    int N = static_cast<int>(circum_verts.size()) - 1;

    if (N % 4 != 0)
        throw Exception("Number of boundary segments ({}) must be divisible by 4 for symmetry.", N);

    // Check constraint: S_innermost >= 4
    // S_k = S_{k+1} - 4, so S_innermost = N - 4 * (n_radial - 1)
    if (N < 4 * n_radial)
        throw Exception(
            "Boundary must have at least {} segments (4 * radial_intervals) to maintain "
            "symmetry and the 4-triangle center constraint.",
            4 * n_radial);

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings(n_radial + 1);
    rings[n_radial] = circum_verts;

    // Cumulative distance along the boundary for interpolation
    std::vector<double> L(N + 1, 0.0);
    for (int i = 1; i <= N; ++i) {
        L[i] = L[i - 1] + utils::distance(circum_verts[i - 1]->point(), circum_verts[i]->point());
    }
    double total_L = L[N];

    // Generate intermediate rings
    for (int k = n_radial - 1; k >= 1; --k) {
        // Each ring has 4 fewer segments than the outer one
        int Sk = N - 4 * (n_radial - k);
        double alpha = static_cast<double>(k) / static_cast<double>(n_radial);

        for (int i = 0; i < Sk; ++i) {
            double l = total_L * (static_cast<double>(i) / Sk);

            // Find j such that L[j] <= l < L[j+1]
            auto it = std::lower_bound(L.begin(), L.end(), l);
            int j = std::distance(L.begin(), it);
            if (j > 0)
                j--;
            if (j >= N)
                j = N - 1;

            double beta = (total_L > 0) ? (l - L[j]) / (L[j + 1] - L[j]) : 0.0;
            auto p_bnd = circum_verts[j]->point() +
                         (circum_verts[j + 1]->point() - circum_verts[j]->point()) * beta;

            auto p = ctr_pnt + (p_bnd - ctr_pnt) * alpha;
            auto uv = gsurf.parameter_from_point(p);
            auto v = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv);
            mesh_surface->add_vertex(v);
            rings[k].push_back(v);
        }
        // Close the ring
        rings[k].push_back(rings[k].front());
    }
    // ring 0 is center
    rings[0] = { ctr };

    // Create triangles

    // center fan (ring 0 -> ring 1)
    auto & r1 = rings[1];
    for (size_t i = 0; i < r1.size() - 1; ++i) {
        mesh_surface->add_triangle(ccw_triangle(gsurf, ctr, r1[i], r1[i + 1]));
    }

    // ring-to-ring strips
    for (int k = 1; k < n_radial; ++k) {
        auto & inner = rings[k];
        auto & outer = rings[k + 1];
        int Sin = static_cast<int>(inner.size()) - 1;
        int Sout = static_cast<int>(outer.size()) - 1;

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
