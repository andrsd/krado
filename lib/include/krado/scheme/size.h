// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme1d.h"

namespace krado {

class SchemeSize : public Scheme, public Scheme1D {
public:
    SchemeSize();
    void mesh_curve(MeshCurve & surface) override;
};

} // namespace krado
