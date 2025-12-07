// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/box.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeBox.hxx"

namespace krado {

Box::Box(const TopoDS_Solid & solid) : GeomVolume(solid)
{
    assign_color();
}

Box
Box::create(const Point & pt1, const Point & pt2)
{
    BRepPrimAPI_MakeBox box(pt1, pt2);
    box.Build();
    if (!box.IsDone())
        throw Exception("Box was not created");
    return Box(box.Solid());
}

} // namespace krado
