// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/tridelaunay.h"
#include "krado/scheme/size.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"

namespace krado {

const std::string SchemeTriDelaunay::name = "tridelaunay";

SchemeTriDelaunay::SchemeTriDelaunay(Options options) :
    SchemeDelaunay(SchemeTriDelaunay::name),
    opts_(options)
{
}

void
SchemeTriDelaunay::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        SchemeSize::Options opts;
        opts.size = this->opts_.max_size;
        curve->set_scheme<SchemeSize>(opts);
    }
}

void
SchemeTriDelaunay::insertion_algo(Ptr<MeshSurface> surface)
{
    // NOTE: gmsh uses 1B (i.e. 1e9) for `max_pnt`
    bowyer_watson(surface, 1000000);
}

} // namespace krado
