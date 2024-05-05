#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"

namespace krado::mesh {

class Mesh;

class MeshCurve {
public:
    MeshCurve(const geo::GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2);

    const geo::GeomCurve & geom_curve() const;

private:
    const geo::GeomCurve & gcurve;
    const MeshVertex * v1;
    const MeshVertex * v2;
};

} // namespace krado::mesh
