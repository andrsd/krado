// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme2d.h"
#include "krado/mesh_curve.h"
#include "krado/log.h"
#include "krado/utils.h"
#include "krado/mesh_surface.h"
#include "krado/scheme/equal.h"

namespace krado {

void
Scheme2D::mesh_surface(Ptr<MeshSurface> surface)
{
    Log::info("Meshing surface {}: scheme='{}'", surface->id(), name());
    on_mesh_surface(surface);

    if (surface->triangles().size() > 0)
        Log::info("- created {} triangles(s)", utils::human_number(surface->triangles().size()));
    if (surface->quadrangles().size() > 0)
        Log::info("- created {} quadrangles(s)",
                  utils::human_number(surface->quadrangles().size()));
}

void
Scheme2D::select_curve_scheme(Ptr<MeshCurve> curve)
{
    on_select_curve_scheme(curve);
}

void
Scheme2D::on_select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        SchemeEqual::Options opt;
        opt.intervals = 1;
        curve->set_scheme<SchemeEqual>(opt);
    }
}

} // namespace krado
