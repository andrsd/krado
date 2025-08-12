// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/axis1.h"
#include "krado/occ.h"

namespace krado {

Axis1::Axis1(const Point & pt, const Vector & dir) : ax1_(occ::to_pnt(pt), occ::to_dir(dir)) {}

Point
Axis1::location() const
{
    auto pnt = this->ax1_.Location();
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Vector
Axis1::direction() const
{
    auto dir = this->ax1_.Direction();
    return Vector(dir.X(), dir.Y(), dir.Z());
}

bool
Axis1::is_equal(const Axis1 & other, double tol) const
{
    return location().is_equal(other.location(), tol) &&
           (direction() - other.direction()).magnitude() <= tol;
}

} // namespace krado
