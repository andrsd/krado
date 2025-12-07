// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/cylinder.h"
#include "krado/exception.h"
#include "krado/geom_volume.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeCylinder.hxx"

namespace krado {

Cylinder::Cylinder(const TopoDS_Solid & solid) : GeomVolume(solid)
{
    assign_color();
}

Cylinder
Cylinder::create(const Axis2 & axis, double radius, double height)
{
    BRepPrimAPI_MakeCylinder cyl(axis, radius, height);
    cyl.Build();
    if (!cyl.IsDone())
        throw Exception("Cylinder was not created");
    return Cylinder(cyl.Solid());
}

} // namespace krado
