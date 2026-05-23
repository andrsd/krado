// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme2d.h"

namespace krado {

class MeshSurface;

class SchemeStructured : public Scheme2D {
public:
    struct Options {};

public:
    SchemeStructured(Options options);

private:
    void on_mesh_surface(Ptr<MeshSurface> surface) override;
};

} // namespace krado
