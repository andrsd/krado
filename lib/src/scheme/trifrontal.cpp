// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/trifrontal.h"
#include "krado/scheme/size.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"

namespace krado {

const std::string SchemeTriFrontal::name = "trifrontal";

SchemeTriFrontal::SchemeTriFrontal(Options options) :
    SchemeDelaunay(SchemeTriFrontal::name),
    opts_(options)
{
}

void
SchemeTriFrontal::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        SchemeSize::Options opts;
        opts.size = this->opts_.max_size;
        curve->set_scheme<SchemeSize>(opts);
    }
}

void
SchemeTriFrontal::insertion_algo(Ptr<MeshSurface> surface)
{
    bowyer_watson_frontal(surface);
}

} // namespace krado
