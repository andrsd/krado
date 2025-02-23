// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <ostream>

namespace krado {

class Point;

/// Vector in 3D space
class Vector {
public:
    /// Construct zero vector
    Vector();
    /// Construct vector with given coordinates
    explicit Vector(double x, double y = 0., double z = 0.);
    /// Construct vector from point
    explicit Vector(const Point & pt);

    [[nodiscard]] double operator()(int idx) const;

    [[nodiscard]] Vector operator+(const Vector & other) const;

    [[nodiscard]] Vector operator-(const Vector & other) const;

    [[nodiscard]] Vector operator*(double alpha) const;

    Vector & operator+=(const Vector & a);

    Vector & operator-=(const Vector & a);

    Vector & operator*=(double alpha);

    /// Compute magnitude of the vector
    ///
    /// @return MAgnitude of the vector
    [[nodiscard]] double magnitude() const;

    /// Normalize the vector
    void normalize();

    /// Return normalized vector
    ///
    /// @return Normalized vector
    [[nodiscard]] Vector normalized() const;

    /// X-component
    double x;
    /// Y-component
    double y;
    /// Z-component
    double z;
};

[[nodiscard]] inline Vector
operator*(double alpha, const Vector & v)
{
    return Vector(v(0) * alpha, v(1) * alpha, v(2) * alpha);
}

[[nodiscard]] inline Vector
operator-(const Vector & a)
{
    return Vector(-a(0), -a(1), -a(2));
}

[[nodiscard]] inline Vector
cross_product(const Vector & a, const Vector & b)
{
    return Vector(a.y * b.z - b.y * a.z, -(a.x * b.z - b.x * a.z), a.x * b.y - b.x * a.y);
}

[[nodiscard]] inline double
dot_product(const Vector & v1, const Vector & v2)
{
    return v1(0) * v2(0) + v1(1) * v2(1) + v1(2) * v2(2);
}

[[nodiscard]] bool operator==(const Vector & a, const Vector & b);

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Vector & vector)
{
    stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return stream;
}
