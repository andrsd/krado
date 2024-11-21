// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <ostream>

namespace krado {

class Point;

class Vector {
public:
    Vector();
    explicit Vector(double x, double y = 0., double z = 0.);
    explicit Vector(const Point & pt);

    double operator()(int idx) const;

    Vector operator+(const Vector & other) const;

    Vector operator-(const Vector & other) const;

    Vector operator*(double alpha) const;

    Vector & operator+=(const Vector & a);

    Vector & operator-=(const Vector & a);

    Vector & operator*=(double alpha);

    /// Compute L2 norm of the vector
    ///
    /// @return L2 norm of the vector
    double norm() const;

    /// Normalize the vector
    void normalize();

    double x, y, z;
};

inline Vector
operator*(double alpha, const Vector & v)
{
    return Vector(v(0) * alpha, v(1) * alpha, v(2) * alpha);
}

inline Vector
operator-(const Vector & a)
{
    return Vector(-a(0), -a(1), -a(2));
}

inline Vector
cross_product(const Vector & a, const Vector & b)
{
    return Vector(a.y * b.z - b.y * a.z, -(a.x * b.z - b.x * a.z), a.x * b.y - b.x * a.y);
}

inline double
dot_product(const Vector & v1, const Vector & v2)
{
    return v1(0) * v2(0) + v1(1) * v2(1) + v1(2) * v2(2);
}

bool operator==(const Vector & a, const Vector & b);

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Vector & vector)
{
    stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return stream;
}
