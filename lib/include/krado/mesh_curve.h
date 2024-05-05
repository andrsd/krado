#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"

namespace krado {

class Mesh;

class MeshCurve {
public:
    MeshCurve(const GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2);

    const GeomCurve & geom_curve() const;

private:
    const GeomCurve & gcurve;
    const MeshVertex * v1;
    const MeshVertex * v2;
};

} // namespace krado
