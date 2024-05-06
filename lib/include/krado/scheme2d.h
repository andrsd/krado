#pragma once

#include "krado/mesh_surface.h"

namespace krado {

/// Base class for 2-dimensional mesh generation schemes
class Scheme2D {
public:
    virtual void mesh_surface(MeshSurface & msurface) = 0;
};

} // namespace krado
