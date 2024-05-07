#pragma once

#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_vertex.h"

namespace krado {

class SchemeFrontalDelaunay : public Scheme, public Scheme2D {
public:
    SchemeFrontalDelaunay();
    void mesh_surface(MeshSurface & surface) override;

    void select_curve_scheme(MeshCurve & curve) override;
};

} // namespace krado
