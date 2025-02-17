// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/ops.h"
#include "krado/geom_curve.h"
#include "krado/exception.h"
#include "Geom_TrimmedCurve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRep_Tool.hxx"

namespace krado {

std::tuple<GeomCurve, GeomCurve>
split_curve(const GeomCurve & curve, Standard_Real split_param)
{
    double umin, umax;
    Handle(Geom_Curve) orig_curve = BRep_Tool::Curve(curve, umin, umax);
    if (split_param < umin || split_param > umax)
        throw Exception("Split parameter {} out of range [{}, {}]", split_param, umin, umax);

    Handle(Geom_Curve) first_part = new Geom_TrimmedCurve(orig_curve, umin, split_param);
    Handle(Geom_Curve) second_part = new Geom_TrimmedCurve(orig_curve, split_param, umax);

    auto first_edge = BRepBuilderAPI_MakeEdge(first_part);
    auto second_edge = BRepBuilderAPI_MakeEdge(second_part);

    return { GeomCurve(first_edge), GeomCurve(second_edge) };
}

} // namespace krado
