// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme/delaunay.h"

namespace krado {

class MeshSurface;
class MeshCurve;

class SchemeTriFrontal : public SchemeDelaunay {
public:
    struct Options {
        double min_size = 0.0;
        double max_size = 0.0;
    };

public:
    SchemeTriFrontal(Options options);
    void select_curve_scheme(Ptr<MeshCurve> curve) override;

private:
    void insertion_algo(Ptr<MeshSurface> surface) override;

    Options opts_;

public:
    static const std::string name;
};

} // namespace krado
