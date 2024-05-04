#pragma once

namespace krado::mesh {

class Point {
public:
    explicit Point(double x, double y = 0., double = 0.);

    double x, y, z;
};

} // namespace krado::mesh
