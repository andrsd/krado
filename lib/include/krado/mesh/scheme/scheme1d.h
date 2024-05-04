#pragma once

#include "krado/mesh/scheme/scheme1d.h"
#include "krado/mesh/curve.h"
#include "krado/geo/curve.h"

namespace krado::mesh {

/// Base class for mesh generation schemes
class Scheme1D {
public:
    Scheme1D();

    virtual mesh::Curve mesh_curve(const geo::Curve & gcurve) = 0;
};

} // namespace krado::mesh
