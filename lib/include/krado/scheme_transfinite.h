// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme1d.h"

namespace krado {

class MeshCurve;

class SchemeTransfinite : public Scheme, public Scheme1D {
public:
    enum Type {
        PROGRESSION = 1,
        BUMP = 2,
        BETA_LAW = 3,
        SIZE_MAP = 4
    };

    SchemeTransfinite();
    void mesh_curve(MeshCurve & curve) override;
};

} // namespace krado
