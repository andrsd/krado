// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme1d.h"

namespace krado {

class SchemeSize : public Scheme1D {
public:
    struct Options {
        /// Number of intervals
        int intervals = 1;
    };

public:
    SchemeSize(Options options);
    void mesh_curve(Ptr<MeshCurve> surface) override;

private:
    Options opts_;
};

} // namespace krado
