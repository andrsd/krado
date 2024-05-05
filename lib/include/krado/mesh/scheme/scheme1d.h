#pragma once

#include "krado/mesh/scheme/scheme1d.h"
#include "krado/mesh/curve.h"

namespace krado::mesh {

/// Base class for mesh generation schemes
class Scheme1D {
public:
    Scheme1D();

    virtual void mesh_curve(const mesh::Curve & mcurve) = 0;
};

} // namespace krado::mesh
