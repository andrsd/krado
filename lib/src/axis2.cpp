// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/axis2.h"
#include "krado/exception.h"
#include "krado/vector.h"
#include "krado/occ.h"

namespace krado {

Axis2::Axis2(const gp_Ax2 & ax2) : ax2_(ax2) {}

Axis2::Axis2(const Point & pt, const Vector & direction) :
    ax2_(occ::to_pnt(pt), occ::to_dir(direction))
{
}

Axis2::Axis2(const Point & pt, const Vector & n, const Vector & v_x)
{
    try {
        this->ax2_ = gp_Ax2(occ::to_pnt(pt), occ::to_dir(n), occ::to_dir(v_x));
    }
    catch (const Standard_ConstructionError & e) {
        throw Exception("Axis2: `n` and `v_x` are parallel");
    }
}

Point
Axis2::location() const
{
    auto pnt = this->ax2_.Location();
    return Point(pnt.X(), pnt.Y(), pnt.Z());
}

Axis1
Axis2::axis() const
{
    auto ax1 = this->ax2_.Axis();
    auto loc = ax1.Location();
    auto dir = ax1.Direction();
    return Axis1(Point(loc.X(), loc.Y(), loc.Z()), Vector(dir.X(), dir.Y(), dir.Z()));
}

Vector
Axis2::direction() const
{
    auto dir = this->ax2_.Direction();
    return Vector(dir.X(), dir.Y(), dir.Z());
}

Vector
Axis2::x_direction() const
{
    auto x_dir = this->ax2_.XDirection();
    return Vector(x_dir.X(), x_dir.Y(), x_dir.Z());
}

Vector
Axis2::y_direction() const
{
    auto y_dir = this->ax2_.YDirection();
    return Vector(y_dir.X(), y_dir.Y(), y_dir.Z());
}

Axis2::operator gp_Ax2() const
{
    return this->ax2_;
}

} // namespace krado
