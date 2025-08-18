// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/geom_volume.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshSurface;

class MeshVolume : public MeshingParameters {
public:
    MeshVolume(const GeomVolume & gvolume, const std::vector<Ptr<MeshSurface>> & mesh_surfaces);

    /// Get the unique identifier of the volume.
    ///
    /// @return The unique identifier of the volume.
    int id() const;

    /// Get geometrical volume associated with this surface
    ///
    /// @return Geometrical volume associated with this surface
    [[nodiscard]] const GeomVolume & geom_volume() const;

    /// Get surfaces bounding this surface
    [[nodiscard]] const std::vector<Ptr<MeshSurface>> & surfaces() const;

private:
    const GeomVolume & gvolume_;
    /// Mesh surfaces bounding this surface
    std::vector<Ptr<MeshSurface>> mesh_surfaces_;
};

} // namespace krado
