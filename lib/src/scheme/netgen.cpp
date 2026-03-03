// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/netgen.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/log.h"
#include <map>

namespace krado {

static const std::string scheme_name = "netgen";

SchemeNetgen::SchemeNetgen(Options options) :
    Scheme3D(scheme_name),
    Scheme2D(scheme_name),
    Scheme1D(scheme_name),
    opts_(options)
{
}

void
SchemeNetgen::mesh_volume(Ptr<MeshVolume> volume)
{
    Log::info("Meshing volume {}: scheme='{}'", volume->id(), scheme_name);
}

void
SchemeNetgen::mesh_surface(Ptr<MeshSurface> surface)
{
    Log::info("Meshing surface {}: scheme='{}'", surface->id(), scheme_name);
}

void
SchemeNetgen::mesh_curve(Ptr<MeshCurve> curve)
{
    Log::info("Meshing curve {}: scheme='{}'", curve->id(), scheme_name);
}

void
SchemeNetgen::select_surface_scheme(Ptr<MeshSurface> surface)
{
    if (!surface->has_scheme())
        surface->set_scheme<SchemeNetgen>(this->opts_);
}

void
SchemeNetgen::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        curve->set_scheme<SchemeNetgen>(this->opts_);
    }
}

} // namespace krado
