// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme1d.h"
#include <vector>

namespace krado {

class SchemePinpoint : public Scheme1D {
public:
    struct Options {
        std::vector<double> positions;
    };

public:
    SchemePinpoint(Options options);

private:
    void on_mesh_curve(Ptr<MeshCurve> surface) override;

    Options opts_;
};

} // namespace krado
