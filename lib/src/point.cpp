// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/point.h"
#include "krado/exception.h"
#include "krado/uv_param.h"
#include "krado/vector.h"
#include "gp_Pnt.hxx"
#include <iostream>
#include <iomanip>

namespace krado {

Point::Point() : x(0.), y(0.), z(0.) {}

Point::Point(const UVParam & uv) : x(uv.u), y(uv.v), z(0.) {}

Point::Point(double x, double y, double z) : x(x), y(y), z(z) {}

bool
Point::is_equal(const Point & other, double tol) const
{
    auto diff = *this - other;
    return diff.magnitude() <= tol;
}

double
Point::operator()(int idx) const
{
    if (idx == 0)
        return this->x;
    else if (idx == 1)
        return this->y;
    else if (idx == 2)
        return this->z;
    else
        throw Exception("Incorrect index");
}

double &
Point::operator()(int idx)
{
    if (idx == 0)
        return this->x;
    else if (idx == 1)
        return this->y;
    else if (idx == 2)
        return this->z;
    else
        throw Exception("Incorrect index");
}

Point
Point::operator+(const Point & b) const
{
    return Point(this->x + b.x, this->y + b.y, this->z + b.z);
}

Point
Point::operator+(const Vector & v) const
{
    Point p = *this;
    p += v;
    return p;
}

Point
Point::operator-(const Vector & v) const
{
    Point p = *this;
    p -= v;
    return p;
}

Vector
Point::operator-(const Point & v) const
{
    return Vector(this->x - v.x, this->y - v.y, this->z - v.z);
}

void
Point::operator+=(const Point & p)
{
    x += p.x;
    y += p.y;
    z += p.z;
}

void
Point::operator+=(const Vector & v)
{
    x += v.x;
    y += v.y;
    z += v.z;
}

void
Point::operator-=(const Point & p)
{
    x -= p.x;
    y -= p.y;
    z -= p.z;
}

void
Point::operator-=(const Vector & v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
}

void
Point::operator*=(double alpha)
{
    x *= alpha;
    y *= alpha;
    z *= alpha;
}

void
Point::operator/=(double div)
{
    x /= div;
    y /= div;
    z /= div;
}

Point
Point::operator*(double alpha)
{
    return Point(x * alpha, y * alpha, z * alpha);
}

double
Point::distance(const Point & p) const
{
    double dx = x - p.x;
    double dy = y - p.y;
    double dz = z - p.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool
Point::operator<(const Point & p) const
{
    if (x < p.x)
        return true;
    if (x > p.x)
        return false;
    if (y < p.y)
        return true;
    if (y > p.y)
        return false;
    if (z < p.z)
        return true;
    return false;
}

void
Point::transform(const std::vector<double> & tfo)
{
    if (tfo.size() < 12)
        throw Exception("Expecting at least 12 entries in 'tfo'.");
    double old[3] = { x, y, z };
    x = y = z = 0.;
    int idx = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            (*this)(i) += old[j] * tfo[idx++];
        (*this)(i) += tfo[idx++];
    }
}

bool
operator==(const krado::Point & a, const krado::Point & b)
{
    return (std::abs(a.x - b.x) < 1e-12) && (std::abs(a.y - b.y) < 1e-12) &&
           (std::abs(a.z - b.z) < 1e-12);
}

Point
Point::create(const gp_Pnt & pt)
{
    return Point(pt.X(), pt.Y(), pt.Z());
}

} // namespace krado
