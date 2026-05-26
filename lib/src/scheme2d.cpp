// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme2d.h"
#include "krado/mesh_curve.h"
#include "krado/log.h"
#include "krado/utils.h"
#include "krado/timer.h"
#include "krado/mesh_surface.h"
#include "krado/scheme/equal.h"

namespace krado {

void
Scheme2D::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        SchemeEqual::Options opt;
        opt.intervals = 1;
        curve->set_scheme<SchemeEqual>(opt);
    }
}

} // namespace krado
