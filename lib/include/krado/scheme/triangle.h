// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme2d.h"
#include "krado/types.h"

namespace krado {

class SchemeTriangle : public Scheme2D {
public:
    struct Options {
        /// Region point
        Optional<std::tuple<double, double>> region_point;
        /// Maximum area
        Optional<double> max_area;
    };

public:
    SchemeTriangle(Options options);
    void mesh_surface(Ptr<MeshSurface> surface) override;

private:
    Options opts_;
};

} // namespace krado
