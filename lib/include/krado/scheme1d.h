#pragma once

#include "krado/scheme1d.h"
#include "krado/mesh_curve.h"

namespace krado::mesh {

/// Base class for mesh generation schemes
class Scheme1D {
public:
    Scheme1D();

    virtual void mesh_curve(const mesh::MeshCurve & mcurve) = 0;
};

} // namespace krado::mesh
