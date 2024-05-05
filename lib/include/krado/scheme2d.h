#pragma once

#include "krado/mesh_surface.h"

namespace krado {

/// Base class for mesh generation schemes
class Scheme2D {
public:
    Scheme2D();

    virtual void mesh_surface(MeshSurface & msurface) = 0;
};

} // namespace krado
