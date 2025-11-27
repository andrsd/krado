// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"

namespace krado {

class MeshSurface;
class MeshCurve;

/// Base class for 2-dimensional mesh generation schemes
class Scheme2D {
public:
    Scheme2D(const std::string & name) : name_(name) {}
    virtual ~Scheme2D() = default;

    /// Mesh surface
    ///
    /// @param surface Surface to mesh
    virtual void mesh_surface(Ptr<MeshSurface> surface) = 0;

    /// Select meshing scheme for a curve
    ///
    /// @param curve Curve to select the scheme for
    virtual void select_curve_scheme(Ptr<MeshCurve> curve);

    const std::string
    name() const
    {
        return this->name_;
    }

private:
    std::string name_;
};

} // namespace krado
