#pragma once

#include "krado/scheme.h"
#include "krado/scheme2d.h"

namespace krado {

class SchemeTriangulo : public Scheme, public Scheme2D {
public:
    SchemeTriangulo(const Parameters & params);
    void mesh_surface(MeshSurface & msurface) override;

private:
    /// Holes positions to pass into the triangulation
    std::vector<Point> holes;
};

} // namespace krado
