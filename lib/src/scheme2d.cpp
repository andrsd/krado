// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme2d.h"
#include "krado/mesh_curve.h"

namespace krado {

void
Scheme2D::select_curve_scheme(MeshCurve & curve)
{
    auto & pars = curve.meshing_parameters();
    auto scheme_name = pars.get<std::string>("scheme");
    if (scheme_name == "auto") {
        pars.set<std::string>("scheme") = "equal";
        pars.set<int>("intervals") = 1;
    }
}

} // namespace krado
