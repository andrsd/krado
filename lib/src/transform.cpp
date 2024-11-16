#include "krado/transform.h"
#include "krado/point.h"
#include <iostream>

namespace krado {

Trsf::Trsf()
{
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            this->mat[i][j] = 0.;
}

Trsf
Trsf::operator*(const Trsf & other) const
{
    Trsf result;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            for (auto k = 0; k < N; k++)
                result.mat[i][j] += other.mat[i][k] * this->mat[k][j];
    return result;
}

Point
Trsf::operator*(const Point & other) const
{
    double result[N] = { 0., 0., 0., 0. };
    double pt[N] = { other.x, other.y, other.z, 1. };
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            result[i] += this->mat[i][j] * pt[j];
    return Point(result[0] / result[3], result[1] / result[3], result[2] / result[3]);
}

Trsf
Trsf::scale(double factor)
{
    auto s = Trsf::identity();
    for (auto i = 0; i < N - 1; i++)
        s.mat[i][i] = factor;
    return s;
}

Trsf
Trsf::scale(double factor_x, double factor_y, double factor_z)
{
    auto s = Trsf::identity();
    s.mat[0][0] = factor_x;
    s.mat[1][1] = factor_y;
    s.mat[2][2] = factor_z;
    return s;
}

Trsf
Trsf::translate(double tx, double ty, double tz)
{
    auto t = Trsf::identity();
    t.mat[0][3] = tx;
    t.mat[1][3] = ty;
    t.mat[2][3] = tz;
    return t;
}

Trsf
Trsf::rotate_x(double theta)
{
    auto r = Trsf::identity();
    r.mat[1][1] = std::cos(theta);
    r.mat[1][2] = -std::sin(theta);
    r.mat[2][1] = std::sin(theta);
    r.mat[2][2] = std::cos(theta);
    return r;
}

Trsf
Trsf::rotate_y(double theta)
{
    auto r = Trsf::identity();
    r.mat[0][0] = std::cos(theta);
    r.mat[0][2] = std::sin(theta);
    r.mat[2][0] = -std::sin(theta);
    r.mat[2][2] = std::cos(theta);
    return r;
}

Trsf
Trsf::rotate_z(double theta)
{
    auto r = Trsf::identity();
    r.mat[0][0] = std::cos(theta);
    r.mat[0][1] = -std::sin(theta);
    r.mat[1][0] = std::sin(theta);
    r.mat[1][1] = std::cos(theta);
    return r;
}

Trsf
Trsf::identity()
{
    Trsf result;
    for (auto i = 0; i < N; i++)
        result.mat[i][i] = 1.;
    return result;
}

} // namespace krado
