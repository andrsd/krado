// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/meshable.h"
#include "krado/scheme3d.h"
#include "krado/ptr.h"
#include "krado/mesh_element.h"
#include <vector>
#include <memory>

namespace krado {

class MeshSurface;
class GeomVolume;

class MeshVolume : public Meshable {
public:
    MeshVolume(ShapeID id,
               const GeomVolume & gvolume,
               const std::vector<Ptr<MeshSurface>> & mesh_surfaces);
    ~MeshVolume();

    /// Get the unique identifier of the volume.
    ///
    /// @return The unique identifier of the volume.
    ShapeID id() const;

    /// Get geometrical volume associated with this surface
    ///
    /// @return Geometrical volume associated with this surface
    [[nodiscard]] const GeomVolume & geom_volume() const;

    /// Get surfaces bounding this surface
    [[nodiscard]] Span<const Ptr<MeshSurface>> surfaces() const;
    [[nodiscard]] Span<Ptr<MeshSurface>> surfaces();

    /// Get mesh elements
    ///
    /// @return Mesh elements
    [[nodiscard]] Span<const MeshElement> tetrahedra() const;

    /// Add a mesh element to the volume
    ///
    /// @param element Mesh element to add
    void add_tetra(const std::array<Ptr<MeshVertexAbstract>, 4> & tetra);

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

    Scheme3D & scheme();

private:
    ///
    ShapeID id_;
    ///
    const GeomVolume & gvolume_;
    /// Mesh surfaces bounding this surface
    std::vector<Ptr<MeshSurface>> mesh_surfaces_;
    /// Mesh elements
    std::vector<MeshElement> tetras_;
    ///
    std::unique_ptr<Scheme3D> scheme_;
};

} // namespace krado
