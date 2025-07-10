// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/scheme.h"
#include "krado/scheme2d.h"

namespace krado {

class MeshSurface;
class MeshCurve;

class SchemeTriCircle : public Scheme, public Scheme2D {
public:
    SchemeTriCircle();

    void mesh_surface(MeshSurface & surface) override;
    void select_curve_scheme(MeshCurve & curve) override;
};

} // namespace krado
