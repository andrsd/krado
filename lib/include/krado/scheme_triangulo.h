#pragma once

#include "krado/scheme2d.h"

namespace krado {

class SchemeTriangulo : public Scheme2D {
public:
    void mesh_surface(MeshSurface & msurface) override;
};

} // namespace krado
