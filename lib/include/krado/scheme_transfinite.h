// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme1d.h"
#include "krado/integral_1d.h"

namespace krado {

class MeshCurve;

class SchemeTransfinite : public Scheme, public Scheme1D {
public:
    SchemeTransfinite(const std::string & name);
    void mesh_curve(MeshCurve & curve) override;

protected:
    virtual Integral1D compute_integral(const MeshCurve & curve) = 0;
};

} // namespace krado
