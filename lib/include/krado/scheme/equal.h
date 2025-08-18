// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme.h"
#include "krado/scheme1d.h"

namespace krado {

class SchemeEqual : public Scheme, public Scheme1D {
public:
    SchemeEqual();
    void mesh_curve(Ptr<MeshCurve> surface) override;
};

} // namespace krado
