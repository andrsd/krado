#pragma once

#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/mesh_curve.h"

namespace krado {

class SchemeTriangulo : public Scheme, public Scheme2D {
public:
    SchemeTriangulo(Mesh & mesh, const Parameters & params);
    void mesh_surface(MeshSurface & surface) override;

private:
    /// Holes positions to pass into the triangulation
    std::vector<Point> holes;
};

} // namespace krado
