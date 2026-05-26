// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme.h"
#include "krado/scheme1d.h"

namespace krado {

class SchemeBias : public Scheme, public Scheme1D {
public:
    struct Options {
        int intervals;
        double factor;
    };

public:
    SchemeBias(Options options);

    void mesh_curve(Ptr<MeshCurve> surface) override;

private:
    std::string params_to_str() override;

    Options opts_;
};

} // namespace krado
