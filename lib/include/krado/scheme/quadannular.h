// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme/annular.h"

namespace krado {

class MeshSurface;
class MeshCurve;

/**
 * @brief QuadAnnular scheme for annular surfaces with constant segment count.
 *
 * This scheme generates a quadrangle-dominant mesh on an annular surface where
 * the inner and outer boundaries have the same number of segments.
 */
class SchemeQuadAnnular : public SchemeAnnular {
public:
    struct Options {
        /// Number of radial intervals (rings of quads)
        int radial_intervals = 1;
    };

public:
    SchemeQuadAnnular(Options options);

    void mesh_surface(Ptr<MeshSurface> surface) override;
    void select_curve_scheme(Ptr<MeshCurve> curve) override;

private:
    std::string params_to_str() override;

    Options opts_;
};

} // namespace krado
