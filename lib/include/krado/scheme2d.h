// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_surface.h"

namespace krado {

class GeomCurve;

/// Base class for 2-dimensional mesh generation schemes
class Scheme2D {
public:
    /// Mesh surface
    ///
    /// @param surface Surface to mesh
    virtual void mesh_surface(MeshSurface & surface) = 0;

    /// Select meshing scheme for a curve
    ///
    /// @param curve Curve to select the scheme for
    virtual void select_curve_scheme(MeshCurve & curve);
};

} // namespace krado
