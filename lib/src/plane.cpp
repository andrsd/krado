// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/plane.h"
#include "krado/axis1.h"
#include "krado/occ.h"

namespace krado {

Plane::Plane(const gp_Pln & pln) : pln_(pln) {}

Plane::Plane(const Point & pt, const Vector & normal) : pln_(occ::to_pnt(pt), occ::to_dir(normal))
{
}

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

} // namespace krado
