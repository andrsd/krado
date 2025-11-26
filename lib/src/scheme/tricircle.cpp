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
#include "krado/log.h"
#include "krado/utils.h"
#include <memory>

namespace krado {

static const std::string scheme_name = "tricircle";

SchemeTriCircle::SchemeTriCircle() : Scheme(scheme_name), Scheme2D() {}

void
SchemeTriCircle::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (curve->scheme().name() == "auto") {
        // minimum of 4 intervals
        curve->set_scheme("equal").set("intervals", 4);
    }
}

void
SchemeTriCircle::mesh_surface(Ptr<MeshSurface> mesh_surface)
{
    Log::info("Meshing surface {}: scheme='tricircle'", mesh_surface->id());

    const auto & gsurf = mesh_surface->geom_surface();
    if (!is_circular_face(gsurf))
        throw Exception("Surface {} is not a circle", gsurf.id());

    auto n_radial = get<int>("radial_intervals");
    if (n_radial <= 0)
        throw Exception("Parameter 'radial_intervals' must be a positive number");

    // Center vertex
    auto geom_crv = gsurf.curves()[0];
    auto ctr_pnt = get_circle_center(geom_crv);
    auto uv_ctr = gsurf.parameter_from_point(ctr_pnt);
    auto ctr = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv_ctr);
    mesh_surface->add_vertex(ctr);

    // Outer ring vertices (existing)
    auto & mesh_crv = mesh_surface->curves()[0];
    for (auto & vtx : mesh_crv->bounding_vertices())
        mesh_surface->add_vertex(vtx);
    for (auto & vtx : mesh_crv->curve_vertices())
        mesh_surface->add_vertex(vtx);

    std::vector<std::vector<Ptr<MeshVertexAbstract>>> rings;
    rings.resize(n_radial + 1);

    // ring `0` contains only center
    rings.push_back({ ctr });

    // ring `n_radial` contains full boundary
    auto circum_verts = mesh_crv->all_vertices();
    rings[n_radial] = circum_verts;

    // Generate intermediate rings
    for (int r = 1; r < n_radial; ++r) {
        auto alpha = static_cast<double>(r) / static_cast<double>(n_radial);

        for (auto & bv : circum_verts) {
            // Linear interpolation of points on the surface along chord direction
            auto p = ctr_pnt + (bv->point() - ctr_pnt) * alpha;

            auto uv = gsurf.parameter_from_point(p);
            auto vr = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv);
            mesh_surface->add_vertex(vr);

            rings[r].push_back(vr);
        }
    }

    // Create triangles

    // center fan (ring 0 -> ring 1)
    for (size_t i = 0; i < rings[1].size(); ++i) {
        size_t j = (i + 1) % rings[1].size();
        mesh_surface->add_triangle(ccw_triangle(gsurf, ctr, rings[1][i], rings[1][j]));
    }

    // ring-to-ring tessellation
    for (int r = 1; r < n_radial; ++r) {
        auto & inner = rings[r];
        auto & outer = rings[r + 1];

        for (size_t i = 0; i < inner.size(); ++i) {
            size_t j = (i + 1) % inner.size();

            auto a = inner[i];
            auto b = inner[j];
            auto c = outer[i];
            auto d = outer[j];

            // split the quad into 2 triangles
            mesh_surface->add_triangle(ccw_triangle(gsurf, a, c, d));
            mesh_surface->add_triangle(ccw_triangle(gsurf, a, d, b));
        }
    }
}

} // namespace krado
