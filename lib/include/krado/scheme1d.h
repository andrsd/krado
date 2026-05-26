// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"

namespace krado {

class MeshCurve;

/// Base class for 1-dimensional mesh generation schemes
class Scheme1D {
public:
    virtual ~Scheme1D() = default;

    virtual void mesh_curve(Ptr<MeshCurve> mcurve) = 0;

protected:
    /// Build segments for a curve
    ///
    /// @param curve Mesh curve
    void build_curve_segments(Ptr<MeshCurve> curve);
};

} // namespace krado
