// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/spline.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "TColgp_HArray1OfPnt.hxx"
#include "GeomAPI_Interpolate.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"

namespace krado {

Spline::Spline(const TopoDS_Edge & edge, Handle(Geom_BSplineCurve) spline) :
    GeomCurve(edge),
    spline_(spline)
{
}

Spline
Spline::create(const std::vector<Point> & points)
{
    auto n = points.size();
    Handle(TColgp_HArray1OfPnt) pnts = new TColgp_HArray1OfPnt(1, n);
    for (std::size_t idx = 0; idx < n; idx++)
        pnts->SetValue(idx + 1, points[idx]);
    GeomAPI_Interpolate mk(pnts, false, 1e-8);
    mk.Perform();
    if (!mk.IsDone())
        throw Exception("Spline was not created");
    BRepBuilderAPI_MakeEdge make_edge(mk.Curve());
    make_edge.Build();
    if (!make_edge.IsDone())
        throw Exception("Edge was not created");
    return Spline(make_edge.Edge(), mk.Curve());
}

Spline
Spline::create(const std::vector<Point> & points,
               const Vector & initial_tg,
               const Vector & final_tg)
{
    auto n = points.size();
    Handle(TColgp_HArray1OfPnt) pnts = new TColgp_HArray1OfPnt(1, n);
    for (int idx = 0; idx < n; idx++)
        pnts->SetValue(idx + 1, points[idx]);
    GeomAPI_Interpolate mk(pnts, false, 1e-8);
    mk.Load(initial_tg, final_tg);
    mk.Perform();
    if (!mk.IsDone())
        throw Exception("Spline was not created");
    BRepBuilderAPI_MakeEdge make_edge(mk.Curve());
    make_edge.Build();
    if (!make_edge.IsDone())
        throw Exception("Edge was not created");
    return Spline(make_edge.Edge(), mk.Curve());
}

} // namespace krado
