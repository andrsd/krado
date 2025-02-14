// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <ostream>

namespace krado {

class Vector;

/// Point in 3D space
class Point {
public:
    /// Construct point at origin
    Point();

    /// Construct point with given coordinates
    explicit Point(double x, double y = 0, double z = 0);

    [[nodiscard]] double operator()(int idx) const;
    double & operator()(int idx);
    [[nodiscard]] Point operator+(const Vector & v) const;
    [[nodiscard]] Point operator-(const Vector & v) const;
    void operator+=(const Point & p);
    void operator+=(const Vector & v);
    void operator-=(const Point & p);
    void operator-=(const Vector & v);
    void operator*=(double alpha);
    void operator/=(double mult);
    [[nodiscard]] Point operator*(double alpha);
    [[nodiscard]] double distance(const Point & p) const;
    // lexicographic
    [[nodiscard]] bool operator<(const Point & p) const;
    void transform(const std::vector<double> & tfo);

    /// X-component
    double x;
    /// Y-component
    double y;
    /// Z-component
    double z;
};

Point operator+(const Point & a, const Point & b);

Vector operator-(const Point & a, const Point & b);

bool operator==(const Point & a, const Point & b);

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Point & pt)
{
    stream << "(x = " << pt.x << ", y = " << pt.y << ", z = " << pt.z << ")";
    return stream;
}
