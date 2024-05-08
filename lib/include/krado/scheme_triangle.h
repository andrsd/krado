#pragma once

#include "krado/config.h"
#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/mesh_curve.h"

namespace krado {

#ifdef KRADO_WITH_TRIANGLE

class SchemeTriangle : public Scheme, public Scheme2D {
public:
    SchemeTriangle(Mesh & mesh, const Parameters & params);
    void mesh_surface(MeshSurface & surface) override;

private:
    /// Holes positions to pass into the triangulation
    std::vector<Point> holes;
};

#else

class SchemeTriangle : public Scheme, public Scheme2D {
public:
    SchemeTriangle(Mesh & mesh, const Parameters & params);
    void mesh_surface(MeshSurface & msurface) override;
};

#endif

} // namespace krado
