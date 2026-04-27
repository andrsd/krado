// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme2d.h"
#include "krado/scheme1d.h"

namespace krado {

class MeshSurface;
class MeshCurve;

class SchemeTriDelaunay : public Scheme2D, public Scheme1D {
public:
    struct Options {
        double min_size = 0.0;
        double max_size = 0.0;
    };

public:
    SchemeTriDelaunay(Options options);
    void mesh_surface(Ptr<MeshSurface> surface) override;
    void mesh_curve(Ptr<MeshCurve> mcurve) override;

    void select_curve_scheme(Ptr<MeshCurve> curve) override;

private:
    Options opts_;

public:
    static const std::string name;
};

} // namespace krado
