// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/arc_of_circle.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "GC_MakeArcOfCircle.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"

namespace krado {

ArcOfCircle::ArcOfCircle(const TopoDS_Edge & edge, Handle(Geom_TrimmedCurve) arc) :
    GeomCurve(edge),
    arc_(arc)
{
}

Point
ArcOfCircle::start_point() const
{
    return Point::create(this->arc_->StartPoint());
}

Point
ArcOfCircle::end_point() const
{
    return Point::create(this->arc_->EndPoint());
}

ArcOfCircle
ArcOfCircle::create(const Point & pt1, const Point & pt2, const Point & pt3)
{
    GC_MakeArcOfCircle mk_circ(pt1, pt2, pt3);
    if (!mk_circ.IsDone())
        throw Exception("ArcOfCircle was not created");
    auto arc = mk_circ.Value();
    BRepBuilderAPI_MakeEdge maker(arc);
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Edge was not created");
    return ArcOfCircle(maker.Edge(), arc);
}

ArcOfCircle
ArcOfCircle::create(const Circle & circ, const Point & pt1, const Point & pt2, bool sense)
{
    GC_MakeArcOfCircle mk_circ(circ, pt1, pt2, sense);
    if (!mk_circ.IsDone())
        throw Exception("ArcOfCircle was not created");
    auto arc = mk_circ.Value();
    BRepBuilderAPI_MakeEdge maker(arc);
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Edge was not created");
    return ArcOfCircle(maker.Edge(), arc);
}

ArcOfCircle
ArcOfCircle::create(const Point & pt1, const Vector & tangent, const Point & pt2)
{
    GC_MakeArcOfCircle mk_circ(pt1, tangent, pt2);
    if (!mk_circ.IsDone())
        throw Exception("ArcOfCircle was not created");
    auto arc = mk_circ.Value();
    BRepBuilderAPI_MakeEdge maker(arc);
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Edge was not created");
    return ArcOfCircle(maker.Edge(), arc);
}

} // namespace krado
