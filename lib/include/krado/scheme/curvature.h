// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme.h"
#include "krado/scheme1d.h"
#include "krado/consts.h"

namespace krado {

class SchemeCurvature : public Scheme, public Scheme1D {
public:
    struct Options {
        /// Minimum mesh size
        double min_size = 0.0;
        /// Maximum mesh size
        double max_size = MAX_LC;
        /// Maximum deflection (in radians)
        double deflection = 0.1;
    };

public:
    SchemeCurvature(Options options);

    void mesh_curve(Ptr<MeshCurve> mcurve) override;

private:
    Options opts_;
};

} // namespace krado
