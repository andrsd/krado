#pragma once

#include "krado/scheme.h"
#include "krado/scheme2d.h"

namespace krado {

class SchemeTriangulo : public Scheme, public Scheme2D {
public:
    SchemeTriangulo(const Parameters & params);
    void mesh_surface(MeshSurface & msurface) override;
};

} // namespace krado
