// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/sphere.h"
#include "krado/exception.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeSphere.hxx"

namespace krado {

namespace {

TopoDS_Solid
make_sphere(const Point & center, double radius)
{
    BRepPrimAPI_MakeSphere sphere(occ::to_pnt(center), radius);
    sphere.Build();
    if (!sphere.IsDone())
        throw Exception("Sphere was not created");
    return sphere.Solid();
}

} // namespace

Sphere::Sphere(const Point & center, double radius) : GeomVolume(make_sphere(center, radius))
{
    assign_color();
}

} // namespace krado
