// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/circle.h"
#include "krado/axis2.h"
#include "krado/exception.h"
#include "krado/geom_curve.h"
#include "krado/occ.h"
#include "GC_MakeCircle.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "gp_Circ.hxx"

namespace krado {

Circle::Circle(const TopoDS_Edge & curve, const gp_Circ & circ) : GeomCurve(curve), circ_(circ) {}

double
Circle::area() const
{
    return this->circ_.Area();
}

double
Circle::radius() const
{
    return this->circ_.Radius();
}

Point
Circle::location() const
{
    auto pnt = this->circ_.Location();
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Circle
Circle::create(const Axis2 & origin, double radius)
{
    GC_MakeCircle make_circ(occ::to_ax2(origin), radius);
    if (!make_circ.IsDone())
        throw Exception("Circle was not created");
    BRepBuilderAPI_MakeEdge maker(make_circ.Value());
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Circle was not created");
    return Circle(maker.Edge(), make_circ.Value()->Circ());
}

Circle
Circle::create(const Point & center, double radius, const Vector & normal)
{
    GC_MakeCircle make_circ(occ::to_pnt(center), occ::to_dir(normal), radius);
    if (!make_circ.IsDone())
        throw Exception("Circle was not created");
    BRepBuilderAPI_MakeEdge maker(make_circ.Value());
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Circle was not created");
    return Circle(maker.Edge(), make_circ.Value()->Circ());
}

Circle
Circle::create(const Point & center, const Point & pt, const Vector & normal)
{
    auto radius = center.distance(pt);
    GC_MakeCircle make_circ(occ::to_pnt(center), occ::to_dir(normal), radius);
    if (!make_circ.IsDone())
        throw Exception("Circle was not created");
    BRepBuilderAPI_MakeEdge maker(make_circ.Value());
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Circle was not created");
    return Circle(maker.Edge(), make_circ.Value()->Circ());
}

Circle
Circle::create(const Point & pt1, const Point & pt2, const Point & pt3)
{
    GC_MakeCircle make_circ(occ::to_pnt(pt1), occ::to_pnt(pt2), occ::to_pnt(pt3));
    if (!make_circ.IsDone())
        throw Exception("Circle was not created");
    BRepBuilderAPI_MakeEdge maker(make_circ.Value());
    maker.Build();
    if (!maker.IsDone())
        throw Exception("Circle was not created");
    return Circle(maker.Edge(), make_circ.Value()->Circ());
}

} // namespace krado
