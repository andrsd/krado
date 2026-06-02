// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme/annular.h"

namespace krado {

class MeshSurface;
class MeshCurve;

/**
 * @brief TriAnnular scheme for annular surfaces with decreasing segment count.
 *
 * This scheme generates a triangular mesh on an annular surface where each
 * inner ring has 6 fewer segments than the ring outside it (hexagonal symmetry).
 */
class SchemeTriAnnular : public SchemeAnnular {
public:
    struct Options {
        /// Number of radial intervals (rings)
        int radial_intervals = 2;
    };

public:
    SchemeTriAnnular(Options options);

    void mesh_surface(Ptr<MeshSurface> surface) override;
    void select_curve_scheme(Ptr<MeshCurve> curve) override;

private:
    std::string params_to_str() override;

    Options opts_;
};

} // namespace krado
