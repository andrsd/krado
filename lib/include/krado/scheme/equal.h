// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme1d.h"

namespace krado {

class SchemeEqual : public Scheme, public Scheme1D {
public:
    SchemeEqual();
    void mesh_curve(MeshCurve & surface) override;
};

} // namespace krado
