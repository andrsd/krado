#pragma once

#include "krado/geo/curve.h"
#include "krado/mesh/vertex.h"

namespace krado::mesh {

class Mesh;

class Curve {
public:
    Curve(const geo::Curve & gcurve, const Vertex * v1, const Vertex * v2);

    const geo::Curve & geom_curve() const;

private:
    const geo::Curve & gcurve;
    const Vertex * v1;
    const Vertex * v2;
};

} // namespace krado::mesh
