// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme1d.h"

namespace krado {

class SchemeEqual : public Scheme1D {
public:
    struct Options {
        int intervals;
    };

public:
    SchemeEqual(Options options);

private:
    void on_mesh_curve(Ptr<MeshCurve> surface) override;

    Options opts_;
};

} // namespace krado
