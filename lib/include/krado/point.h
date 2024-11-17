// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>
#include <ostream>

namespace krado {

class Point {
public:
    Point();
    explicit Point(double x, double y = 0, double z = 0);

    double operator()(int idx) const;
    double & operator()(int idx);
    void operator+=(const Point & p);
    void operator-=(const Point & p);
    void operator*=(double alpha);
    void operator/=(double mult);
    Point operator*(double alpha);
    double distance(const Point & p) const;
    // lexicographic
    bool operator<(const Point & p) const;
    bool transform(const std::vector<double> & tfo);

    double x, y, z;
};

inline Point
operator+(const Point & a, const Point & b)
{
    return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Point
operator-(const Point & a, const Point & b)
{
    return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

bool operator==(const Point & a, const Point & b);

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Point & pt)
{
    stream << "(x = " << pt.x << ", y = " << pt.y << ", z = " << pt.z << ")";
    return stream;
}
