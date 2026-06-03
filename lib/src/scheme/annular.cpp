// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/annular.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/mesh_surface.h"
#include "krado/geom_curve.h"
#include "krado/mesh_curve.h"
#include "krado/utils.h"

namespace krado {

SchemeAnnular::SchemeAnnular(const std::string & name) : Scheme(name), Scheme2D() {}

Point
SchemeAnnular::find_center_point(Ptr<MeshSurface> mesh_surface,
                                 const std::vector<std::vector<Ptr<MeshVertexAbstract>>> & loops)
{
    for (auto & mesh_crv : mesh_surface->curves()) {
        if (mesh_crv->geom_curve().type() == GeomCurve::CurveType::Circle)
            return get_circle_center(mesh_crv->geom_curve());
    }

    Point ctr_pnt(0, 0, 0);
    int count = 0;
    for (const auto & loop : loops) {
        for (const auto & v : loop) {
            ctr_pnt += v->point();
            count++;
        }
    }
    if (count > 0)
        ctr_pnt *= 1.0 / count;
    return ctr_pnt;
}

std::vector<std::vector<Ptr<MeshVertexAbstract>>>
SchemeAnnular::get_boundary_loops(Ptr<MeshSurface> surface)
{
    std::vector<std::vector<Ptr<MeshVertexAbstract>>> loops;
    auto mesh_curves = surface->curves();
    std::vector<bool> visited(mesh_curves.size(), false);

    while (true) {
        int start_idx = -1;
        for (int i = 0; i < (int) mesh_curves.size(); ++i) {
            if (!visited[i]) {
                start_idx = i;
                break;
            }
        }
        if (start_idx == -1)
            break;

        std::vector<Ptr<MeshVertexAbstract>> loop;
        int curr_idx = start_idx;
        visited[curr_idx] = true;

        auto curr_crv = mesh_curves[curr_idx];
        auto vtxs = get_mesh_curve_vertices(curr_crv);
        loop = vtxs;

        while (loop.front() != loop.back()) {
            auto last_v = loop.back();
            bool found_next = false;
            for (int i = 0; i < (int) mesh_curves.size(); ++i) {
                if (!visited[i]) {
                    auto next_crv = mesh_curves[i];
                    auto next_vtxs = get_mesh_curve_vertices(next_crv);
                    if (next_vtxs.front() == last_v) {
                        loop.insert(loop.end(), next_vtxs.begin() + 1, next_vtxs.end());
                        visited[i] = true;
                        found_next = true;
                        break;
                    }
                    else if (next_vtxs.back() == last_v) {
                        std::reverse(next_vtxs.begin(), next_vtxs.end());
                        loop.insert(loop.end(), next_vtxs.begin() + 1, next_vtxs.end());
                        visited[i] = true;
                        found_next = true;
                        break;
                    }
                }
            }
            if (!found_next)
                break;
        }
        loops.push_back(loop);
    }
    return loops;
}

Point
SchemeAnnular::interpolate_loop(const std::vector<Ptr<MeshVertexAbstract>> & loop,
                                const std::vector<double> & L,
                                double l)
{
    int N = static_cast<int>(loop.size()) - 1;
    auto it = std::lower_bound(L.begin(), L.end(), l);
    int j = static_cast<int>(std::distance(L.begin(), it));
    if (j > 0)
        j--;
    if (j >= N)
        j = N - 1;
    double beta = (L[j + 1] > L[j]) ? (l - L[j]) / (L[j + 1] - L[j]) : 0.0;
    return loop[j]->point() + (loop[j + 1]->point() - loop[j]->point()) * beta;
};

std::vector<double>
SchemeAnnular::get_L(const std::vector<Ptr<MeshVertexAbstract>> & loop)
{
    int N = static_cast<int>(loop.size()) - 1;
    std::vector<double> L(N + 1, 0.0);
    for (int i = 1; i <= N; ++i) {
        L[i] = L[i - 1] + utils::distance(loop[i - 1]->point(), loop[i]->point());
    }
    return L;
};

} // namespace krado
