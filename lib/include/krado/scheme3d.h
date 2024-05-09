// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_volume.h"

namespace krado {

/// Base class for 3-dimensional mesh generation schemes
class Scheme3D {
public:
    virtual void mesh_volume(MeshVolume & mvolume) = 0;
};

} // namespace krado
