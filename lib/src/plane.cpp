// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/plane.h"
#include "krado/axis1.h"
#include "krado/geom_surface.h"
#include "krado/occ.h"
#include "BRepAdaptor_Surface.hxx"

namespace krado {

Plane::Plane(const gp_Pln & pln) : pln_(pln) {}

Plane::Plane(const Point & pt, const Vector & normal) : pln_(pt, occ::to_dir(normal)) {}

Point
Plane::location() const
{
    auto pt = this->pln_.Location();
    return Point(pt.X(), pt.Y(), pt.Z());
}

Axis1
Plane::axis() const
{
    return Axis1(this->pln_.Axis());
}

Axis1
Plane::x_axis() const
{
    return Axis1(this->pln_.XAxis());
}

Axis1
Plane::y_axis() const
{
    return Axis1(this->pln_.YAxis());
}

Plane::operator gp_Pln() const
{
    return this->pln_;
}

Plane
Plane::create(const GeomSurface & surface)
{
    BRepAdaptor_Surface pln(surface, true);
    return Plane(pln.Plane());
}

} // namespace krado
