#pragma once

#include "krado/geom_volume.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshSurface;

class MeshVolume : public MeshingParameters {
public:
    MeshVolume(const GeomVolume & gvolume, const std::vector<const MeshSurface *> & mesh_surfaces);

    /// Get geometrical volume associated with this surface
    ///
    /// @return Geometrical volume associated with this surface
    const GeomVolume & geom_volume() const;

    /// Get surfaces bounding this surface
    const std::vector<const MeshSurface *> & surfaces() const;

private:
    const GeomVolume & gvolume;
    /// Mesh surfaces bounding this surface
    std::vector<const MeshSurface *> mesh_surfaces;
};

} // namespace krado
