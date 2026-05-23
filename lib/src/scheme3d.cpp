// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme3d.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_volume.h"
#include "krado/log.h"

namespace krado {

void
Scheme3D::mesh_volume(Ptr<MeshVolume> volume)
{
    Log::info("Meshing volume {}: scheme='{}'", volume->id(), name());
    on_mesh_volume(volume);
}

void
Scheme3D::select_surface_scheme(Ptr<MeshSurface> surface)
{
    on_select_surface_scheme(surface);
}

void
Scheme3D::on_select_surface_scheme(Ptr<MeshSurface> /*surface*/)
{
}

} // namespace krado
