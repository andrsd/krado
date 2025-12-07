// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/sphere.h"
#include "krado/exception.h"
#include "krado/geom_volume.h"
#include "krado/occ.h"
#include "BRepPrimAPI_MakeSphere.hxx"

namespace krado {

Sphere::Sphere(const TopoDS_Solid & solid) : GeomVolume(solid)
{
    assign_color();
}

Sphere
Sphere::create(const Point & center, double radius)
{
    BRepPrimAPI_MakeSphere sphere(center, radius);
    sphere.Build();
    if (!sphere.IsDone())
        throw Exception("Sphere was not created");
    return Sphere(sphere.Solid());
}

} // namespace krado
