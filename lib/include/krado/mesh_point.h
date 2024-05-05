#pragma once

namespace krado {

class MeshPoint {
public:
    explicit MeshPoint(double x, double y = 0., double = 0.);

    double x, y, z;
};

} // namespace krado
