// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/tricircle.h"
#include "krado/exception.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/log.h"

namespace krado {

static const std::string scheme_name = "tricircle";

SchemeTriCircle::SchemeTriCircle() : Scheme(scheme_name), Scheme2D() {}

void
SchemeTriCircle::select_curve_scheme(MeshCurve & curve)
{
    if (curve.scheme().name() == "auto") {
        // minimum of 4 intervals
        curve.set_scheme("equal").set("intervals", 4);
    }
}

void
SchemeTriCircle::mesh_surface(MeshSurface & surface)
{
    Log::info("Meshing surface {}: scheme='tricircle'", surface.id());

    const auto & gsurf = surface.geom_surface();
    if (!is_circular_face(gsurf))
        throw Exception("Surface {} is not a circle", gsurf.id());

    auto geom_crv = gsurf.curves()[0];
    auto ctr_pnt = get_circle_center(geom_crv);
    auto uv = gsurf.parameter_from_point(ctr_pnt);
    auto ctr = new MeshSurfaceVertex(gsurf, uv);
    surface.add_vertex(ctr);
    auto nrm = gsurf.normal(uv);

    auto & mesh_crv = surface.curves()[0];
    auto & circum_verts = mesh_crv->all_vertices();
    for (std::size_t i = 0; i < circum_verts.size(); ++i) {
        std::size_t j = (i + 1) % circum_verts.size();
        auto t = utils::ccw_triangle(ctr, circum_verts[i], circum_verts[j], nrm);
        surface.add_triangle(t);
    }

    //
    for (auto & vtx : mesh_crv->bounding_vertices())
        surface.add_vertex(vtx);
    for (auto & vtx : mesh_crv->curve_vertices())
        surface.add_vertex(vtx);
}

} // namespace krado
