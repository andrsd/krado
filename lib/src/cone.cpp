// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/cone.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeCone.hxx"

namespace krado {

Cone::Cone(const TopoDS_Solid & solid) : GeomVolume(solid)
{
    assign_color();
}

Cone
Cone::create(const Axis2 & location, double radius1, double radius2, double height)
{
    BRepPrimAPI_MakeCone cone(occ::to_ax2(location), radius1, radius2, height);
    cone.Build();
    if (!cone.IsDone())
        throw Exception("Cone was not created");
    return Cone(cone.Solid());
}

} // namespace krado
