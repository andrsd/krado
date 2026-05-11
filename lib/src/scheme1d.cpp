// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme1d.h"
#include "krado/exception.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/ptr.h"

namespace krado {

void
Scheme1D::build_curve_segments(Ptr<MeshCurve> curve)
{
    auto bnd_verts = curve->bounding_vertices();
    if (bnd_verts.size() != 2)
        throw Exception("Curve {} must have 2 bounding vertices", curve->id());

    std::vector<Ptr<MeshVertexAbstract>> all;
    all.push_back(static_ptr_cast<MeshVertexAbstract>(bnd_verts[0]));
    for (auto & cv : curve->curve_vertices())
        all.push_back(static_ptr_cast<MeshVertexAbstract>(cv));
    all.push_back(static_ptr_cast<MeshVertexAbstract>(bnd_verts[1]));

    for (std::size_t i = 0; i + 1 < all.size(); ++i)
        curve->add_segment({ all[i], all[i + 1] });
}

} // namespace krado
