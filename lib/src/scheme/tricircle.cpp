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
#include "krado/predicates.h"
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

    auto geom_crv = gsurf.curves()[0];
    auto ctr_pnt = get_circle_center(geom_crv);
    auto uv_ctr = gsurf.parameter_from_point(ctr_pnt);
    auto ctr = Ptr<MeshSurfaceVertex>::alloc(gsurf, uv_ctr);
    mesh_surface->add_vertex(ctr);

    auto & mesh_crv = mesh_surface->curves()[0];
    for (auto & vtx : mesh_crv->bounding_vertices())
        mesh_surface->add_vertex(vtx);
    for (auto & vtx : mesh_crv->curve_vertices())
        mesh_surface->add_vertex(vtx);

    auto & circum_verts = mesh_crv->all_vertices();
    for (std::size_t i = 0; i < circum_verts.size(); ++i) {
        std::size_t j = (i + 1) % circum_verts.size();

        auto uv_i = gsurf.parameter_from_point(circum_verts[i]->point());
        auto uv_j = gsurf.parameter_from_point(circum_verts[j]->point());
        auto orientation = orient2d(uv_ctr, uv_i, uv_j);

        if (orientation > 0)
            mesh_surface->add_triangle({ ctr, circum_verts[i], circum_verts[j] });
        else if (orientation < 0)
            mesh_surface->add_triangle({ ctr, circum_verts[j], circum_verts[i] });
        else
            Log::error("Degenerate triangle detected. Points are collinear.");
    }
}

} // namespace krado
