#pragma once

#include <ostream>

namespace krado {

class Vector {
public:
    Vector();
    explicit Vector(double x, double y = 0., double z = 0.);

    double operator()(int idx) const;

    double x, y, z;
};

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::Vector & vector)
{
    stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return stream;
}
