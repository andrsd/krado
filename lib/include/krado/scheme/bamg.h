// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme2d.h"

namespace krado {

class SchemeBAMG : public Scheme2D {
public:
    struct Options {
        /// Maximum area
        double max_area;
    };

public:
    SchemeBAMG(Options options);
    void mesh_surface(Ptr<MeshSurface> surface) override;

private:
    Options opts_;
};

} // namespace krado
