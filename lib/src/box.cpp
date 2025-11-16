// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/box.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeBox.hxx"

namespace krado {
namespace {

TopoDS_Solid
make_box(const Point & pt1, const Point & pt2)
{
    BRepPrimAPI_MakeBox box(occ::to_pnt(pt1), occ::to_pnt(pt2));
    box.Build();
    if (!box.IsDone())
        throw Exception("Box was not created");
    return box.Solid();
}

} // namespace

Box::Box(const Point & pt1, const Point & pt2) : GeomVolume(make_box(pt1, pt2))
{
    assign_color();
}

} // namespace krado
