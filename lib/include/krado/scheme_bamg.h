// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/config.h"
#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/point.h"

namespace krado {

class SchemeBAMG : public Scheme, public Scheme2D {
public:
    SchemeBAMG();
    void mesh_surface(MeshSurface & surface) override;
};

} // namespace krado
