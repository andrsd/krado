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
    virtual void mesh_curve(Ptr<MeshCurve> mcurve) = 0;

    const std::string
    name() const
    {
        return this->name_;
    }

private:
    std::string name_;
};

} // namespace krado
