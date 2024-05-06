#pragma once

#include <ostream>

namespace krado {

class MeshPoint {
public:
    explicit MeshPoint(double x, double y = 0., double = 0.);

    double x, y, z;
};

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::MeshPoint & pt)
{
    stream << "(" << pt.x << ", " << pt.y << ", " << pt.z << ")";
    return stream;
}
