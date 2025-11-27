// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/geom_volume.h"
#include "krado/meshable.h"
#include "krado/scheme3d.h"

namespace krado {

class MeshSurface;

class MeshVolume : public Meshable {
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

    /// Set meshing scheme
    ///
    /// @param name Name od the scheme to assign
    /// @return Pointer to the scheme
    template <typename SCHEME>
    SCHEME &
    set_scheme(SCHEME::Options options)
    {
        auto sch = std::make_unique<SCHEME>(options);
        auto sch_ptr = sch.get();
        this->scheme_ = std::move(sch);
        return *sch_ptr;
    }

    Scheme3D &
    scheme()
    {
        return *this->scheme_.get();
    }

private:
    const GeomVolume & gvolume_;
    /// Mesh surfaces bounding this surface
    std::vector<Ptr<MeshSurface>> mesh_surfaces_;
    ///
    std::unique_ptr<Scheme3D> scheme_;
};

} // namespace krado
