// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_curve_vertex.h"

namespace krado {

MeshCurveVertex::MeshCurveVertex(const GeomCurve & geom_curve, double u) :
    MeshVertexAbstract(),
    gcurve(geom_curve),
    u(u)
{
    this->phys_pt = geom_curve.point(u);
}

const GeomCurve &
MeshCurveVertex::geom_curve() const
{
    return this->gcurve;
}

double
MeshCurveVertex::parameter() const
{
    return this->u;
}

Point
MeshCurveVertex::point() const
{
    return this->phys_pt;
}

} // namespace krado
