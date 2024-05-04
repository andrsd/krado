#include "krado/vector.h"
#include "krado/exception.h"

namespace krado {

Vector::Vector() : x(0.), y(0.), z(0.) {}

Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

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

} // namespace krado
