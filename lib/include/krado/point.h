#pragma once

namespace krado {

class Point {
public:
    Point();
    explicit Point(double x, double y = 0, double z = 0);

    double x, y, z;
};

} // namespace krado
