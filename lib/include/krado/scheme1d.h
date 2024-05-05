#pragma once

#include "krado/mesh_curve.h"

namespace krado {

/// Base class for mesh generation schemes
class Scheme1D {
public:
    Scheme1D();

    virtual void mesh_curve(MeshCurve & mcurve) = 0;
};

} // namespace krado
