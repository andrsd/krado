// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"

namespace krado {

class MeshCurve;

/// Base class for 1-dimensional mesh generation schemes
class Scheme1D {
public:
    Scheme1D(const std::string & name) : name_(name) {}
    virtual ~Scheme1D() = default;

    void mesh_curve(Ptr<MeshCurve> mcurve);

    const std::string
    name() const
    {
        return this->name_;
    }

protected:
    virtual void on_mesh_curve(Ptr<MeshCurve> mcurve) = 0;

    /// Build segments for a curve
    ///
    /// @param curve Mesh curve
    void build_curve_segments(Ptr<MeshCurve> curve);

private:
    std::string name_;
};

} // namespace krado
