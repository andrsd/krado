#pragma once

namespace krado {

class Vector {
public:
    Vector();
    explicit Vector(double x, double y = 0., double z = 0.);

    double x, y, z;
};

} // namespace krado
