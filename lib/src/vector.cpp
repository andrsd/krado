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

double
Vector::norm() const
{
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

void
Vector::normalize()
{
    double n = norm();
    if (n) {
        this->x /= n;
        this->y /= n;
        this->z /= n;
    }
}

Vector &
Vector::operator+=(const Vector & a)
{
    this->x += a.x;
    this->y += a.y;
    this->z += a.z;
    return *this;
}

} // namespace krado
