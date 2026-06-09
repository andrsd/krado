// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <ostream>

class gp_Pnt;

namespace krado {

class Vector;
class UVParam;
class Axis1;
class Axis2;

/// Point in 3D space
class Point {
public:
    /// Construct point at origin
    Point();

    explicit Point(UVParam uv);

    /// Construct point with given coordinates
    explicit Point(double x, double y = 0, double z = 0);

    explicit Point(gp_Pnt pt);

    /// Check if two points are equal within a tolerance
    ///
    /// @param other Other point
    /// @param tol Tolerance
    /// @return `true` if points are equal within tolerance
    bool is_equal(const Point & other, double tol = 1e-15) const;

    [[nodiscard]] double operator()(int idx) const;
    double & operator()(int idx);

    /// Add two points
    ///
    /// @param v Point to add
    /// @return Point (this + v)
    [[nodiscard]] Point operator+(const Point & v) const;

    [[nodiscard]] Point operator+(const Vector & v) const;
    [[nodiscard]] Point operator-(const Vector & v) const;

    /// Compute vector from two points
    ///
    /// @param a First point
    /// @param b Second point
    /// @return Vector (this - b)
    [[nodiscard]] Vector operator-(const Point & v) const;

    void operator+=(const Point & p);
    void operator+=(const Vector & v);
    void operator-=(const Point & p);
    void operator-=(const Vector & v);
    void operator*=(double alpha);
    void operator/=(double mult);
    [[nodiscard]] Point operator*(double alpha);
    [[nodiscard]] double distance(Point p) const;
    // lexicographic
    [[nodiscard]] bool operator<(const Point & p) const;
    void transform(const std::vector<double> & tfo);

    void mirror(const Point & pt);
    void mirror(const Axis1 & ax1);
    void mirror(const Axis2 & ax2);

    Point mirrored(const Point & pt) const;
    Point mirrored(const Axis1 & ax1) const;
    Point mirrored(const Axis2 & ax2) const;

    void rotate(const Axis1 & ax1, double angle);
    Point rotated(const Axis1 & ax1, double angle) const;

    void scale(const Point & pt, double s);
    Point scaled(const Point & pt, double s) const;

    void translate(const Vector & vec);
    void translate(const Point & p1, const Point & p2);

    Point translated(const Vector & vec) const;
    Point translated(const Point & p1, const Point & p2) const;

    operator gp_Pnt() const;

    /// X-component
    double x;
    /// Y-component
    double y;
    /// Z-component
    double z;

public:
    static Point create(const gp_Pnt & pt);
};

inline Point
operator*(double alpha, const Point & pt)
{
    return Point(pt(0) * alpha, pt(1) * alpha, pt(2) * alpha);
}

bool operator==(const Point & a, const Point & b);

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Point & pt)
{
    stream << "(x = " << pt.x << ", y = " << pt.y << ", z = " << pt.z << ")";
    return stream;
}
