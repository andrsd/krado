#pragma once

#include <ostream>

namespace krado {

class Point;

class MeshPoint {
public:
    explicit MeshPoint(double x, double y = 0., double = 0.);
    explicit MeshPoint(const Point & pt);

    double x, y, z;
};

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::MeshPoint & pt)
{
    stream << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
    return stream;
}
