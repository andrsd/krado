// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"

namespace krado {

class MeshVolume;
class MeshSurface;

/// Base class for 3-dimensional mesh generation schemes
class Scheme3D {
public:
    /// Mesh a volume
    ///
    /// @param volume Volume to mesh
    virtual void mesh_volume(Ptr<MeshVolume> volume) = 0;

    /// Select meshing scheme for a surface
    ///
    /// @param surface Surface to mesh
    virtual void select_surface_scheme(Ptr<MeshSurface> surface);
};

} // namespace krado
