// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/cylinder.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeCylinder.hxx"

namespace krado {
namespace {

TopoDS_Solid
make_cylinder(const Axis2 & axis, double radius, double height)
{
    BRepPrimAPI_MakeCylinder cyl(occ::to_ax2(axis), radius, height);
    cyl.Build();
    if (!cyl.IsDone())
        throw Exception("Cylinder was not created");
    return cyl.Solid();
}

} // namespace

Cylinder::Cylinder(const Axis2 & axis, double radius, double height) :
    GeomVolume(make_cylinder(axis, radius, height))
{
    assign_color();
}

} // namespace krado
