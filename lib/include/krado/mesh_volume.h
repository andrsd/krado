// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshSurface;

class MeshVolume : public MeshingParameters {
public:
    MeshVolume(const GeomVolume & gvolume, const std::vector<MeshSurface *> & mesh_surfaces);

    /// Get geometrical volume associated with this surface
    ///
    /// @return Geometrical volume associated with this surface
    const GeomVolume & geom_volume() const;

    /// Get surfaces bounding this surface
    const std::vector<MeshSurface *> & surfaces() const;

    /// Check if the volume is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark volume as meshed
    void set_meshed();

private:
    const GeomVolume & gvolume;
    /// Mesh surfaces bounding this surface
    std::vector<MeshSurface *> mesh_surfaces;
    /// Flag indicating if the volume is meshed
    bool meshed;
};

} // namespace krado
