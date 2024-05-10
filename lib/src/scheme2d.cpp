// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme2d.h"
#include "krado/scheme.h"
#include "krado/mesh_curve.h"

namespace krado {

void
Scheme2D::select_curve_scheme(MeshCurve & curve)
{
    auto scheme = curve.get_scheme();
    if (scheme.name() == "auto") {
        curve
            .set_scheme("equal")
            .set("intervals", 1);
    }
}

} // namespace krado
