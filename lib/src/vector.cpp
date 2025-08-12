// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/vector.h"
#include "krado/point.h"
#include "krado/exception.h"
#include "krado/axis1.h"
#include "krado/occ.h"

namespace krado {

Vector::Vector() : x(0.), y(0.), z(0.) {}

Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

Vector::Vector(const Point & pt) : x(pt.x), y(pt.y), z(pt.z) {}

double
Vector::operator()(int idx) const
{
    if (idx == 0)
        return this->x;
    else if (idx == 1)
        return this->y;
    else if (idx == 2)
        return this->z;
    else
        throw Exception("'idx' can be only 0, 1, or 2.");
}

double
Vector::magnitude() const
{
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void
Vector::normalize()
{
    double n = magnitude();
    if (n) {
        this->x /= n;
        this->y /= n;
        this->z /= n;
    }
}

Vector
Vector::normalized() const
{
    Vector v = *this;
    v.normalize();
    return v;
}

void
Vector::rotate(const Axis1 & axis, double angle)
{
    auto ax1 = occ::to_ax1(axis);
    auto v = occ::to_vec(*this);
    v.Rotate(ax1, angle);
    this->x = v.X();
    this->y = v.Y();
    this->z = v.Z();
}

Vector
Vector::rotated(const Axis1 & axis, double angle) const
{
    auto ax1 = occ::to_ax1(axis);
    auto v = occ::to_vec(*this);
    v.Rotate(ax1, angle);
    return Vector(v.X(), v.Y(), v.Z());
}

Vector
Vector::operator+(const Vector & other) const
{
    return Vector(this->x + other.x, this->y + other.y, this->z + other.z);
}

Vector
Vector::operator-(const Vector & other) const
{
    return Vector(this->x - other.x, this->y - other.y, this->z - other.z);
}

Vector
Vector::operator*(double alpha) const
{
    return Vector(this->x * alpha, this->y * alpha, this->z * alpha);
}

Vector &
Vector::operator+=(const Vector & a)
{
    this->x += a.x;
    this->y += a.y;
    this->z += a.z;
    return *this;
}

Vector &
Vector::operator-=(const Vector & a)
{
    this->x -= a.x;
    this->y -= a.y;
    this->z -= a.z;
    return *this;
}

Vector &
Vector::operator*=(double alpha)
{
    this->x *= alpha;
    this->y *= alpha;
    this->z *= alpha;
    return *this;
}

bool
operator==(const Vector & a, const Vector & b)
{
    return (std::abs(a.x - b.x) < 1e-15) && (std::abs(a.y - b.y) < 1e-15) &&
           (std::abs(a.z - b.z) < 1e-15);
}

} // namespace krado
