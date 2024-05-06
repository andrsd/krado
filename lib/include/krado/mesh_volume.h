#pragma once

#include "krado/geom_volume.h"

namespace krado {

class MeshSurface;

class MeshVolume {
public:
    MeshVolume(const GeomVolume & gvolume, const std::vector<const MeshSurface *> & mesh_surfaces);

    /// Get geometrical volume associated with this surface
    ///
    /// @return Geometrical volume associated with this surface
    const GeomVolume & geom_volume() const;

    /// Get volume marker
    ///
    /// @return Volume marker
    int marker() const;

    /// Set volume marker
    ///
    /// @param marker New volume marker
    void set_marker(int marker);

    /// Get surfaces bounding this surface
    const std::vector<const MeshSurface *> & surfaces() const;

private:
    const GeomVolume & gvolume;
    /// Surface marker
    int volume_marker;
    /// Mesh surfaces bounding this surface
    std::vector<const MeshSurface *> mesh_surfaces;
};

} // namespace krado
