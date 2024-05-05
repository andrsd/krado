#pragma once

namespace krado::mesh {

class MeshPoint {
public:
    explicit MeshPoint(double x, double y = 0., double = 0.);

    double x, y, z;
};

} // namespace krado::mesh
