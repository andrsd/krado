// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_curve_vertex.h"
#include "krado/geom_curve.h"

namespace krado {

MeshCurveVertex::MeshCurveVertex(const GeomCurve & geom_curve, double u) :
    MeshVertexAbstract(geom_curve),
    gcurve_(geom_curve),
    u_(u)
{
    this->phys_pt_ = geom_curve.point(u);
}

const GeomCurve &
MeshCurveVertex::geom_curve() const
{
    return this->gcurve_;
}

double
MeshCurveVertex::parameter() const
{
    return this->u_;
}

Point
MeshCurveVertex::point() const
{
    return this->phys_pt_;
}

} // namespace krado
