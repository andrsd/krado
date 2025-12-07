// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/line.h"
#include "krado/point.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "BRepBuilderAPI_MakeEdge.hxx"

namespace krado {

Line::Line(const TopoDS_Edge & curve) : GeomCurve(curve) {}

Line
Line::create(const Point & pt1, const Point & pt2)
{
    BRepBuilderAPI_MakeEdge maker(occ::to_pnt(pt1), occ::to_pnt(pt2));
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Edge was not created");
    return Line(maker.Edge());
}

} // namespace krado
