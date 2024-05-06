#pragma once

#include "krado/mesh_curve.h"

namespace krado {

/// Base class for 1-dimensional mesh generation schemes
class Scheme1D {
public:
    virtual void mesh_curve(MeshCurve & mcurve) = 0;
};

} // namespace krado
