// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme.h"
#include "krado/scheme2d.h"

namespace krado {

class SchemeTriangle : public Scheme, public Scheme2D {
public:
    SchemeTriangle();
    void mesh_surface(Ptr<MeshSurface> surface) override;
};

} // namespace krado
