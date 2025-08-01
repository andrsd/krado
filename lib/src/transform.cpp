// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/transform.h"
#include "krado/point.h"
#include <iostream>
#include <cmath>

namespace krado {

Trsf::Trsf()
{
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            this->mat_[i][j] = 0.;
}

Trsf &
Trsf::scale(double factor)
{
    for (auto i = 0; i < N - 1; i++)
        this->mat_[i][i] *= factor;
    return *this;
}

Trsf &
Trsf::scale(double factor_x, double factor_y, double factor_z)
{
    this->mat_[0][0] *= factor_x;
    this->mat_[1][1] *= factor_y;
    this->mat_[2][2] *= factor_z;
    return *this;
}

Trsf &
Trsf::translate(double tx, double ty, double tz)
{
    this->mat_[0][3] += tx;
    this->mat_[1][3] += ty;
    this->mat_[2][3] += tz;
    return *this;
}

Trsf &
Trsf::rotate_x(double theta)
{
    *this *= Trsf::rotated_x(theta);
    return *this;
}

Trsf &
Trsf::rotate_y(double theta)
{
    *this *= Trsf::rotated_y(theta);
    return *this;
}

Trsf &
Trsf::rotate_z(double theta)
{
    *this *= Trsf::rotated_z(theta);
    return *this;
}

Trsf
Trsf::operator*(const Trsf & other) const
{
    Trsf result;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            for (auto k = 0; k < N; k++)
                result.mat_[i][j] += other.mat_[i][k] * this->mat_[k][j];
    return result;
}

Trsf &
Trsf::operator*=(const Trsf & other)
{
    Trsf result;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            for (auto k = 0; k < N; k++)
                result.mat_[i][j] += other.mat_[i][k] * this->mat_[k][j];
    *this = result;
    return *this;
}

Point
Trsf::operator*(const Point & other) const
{
    double result[N] = { 0., 0., 0., 0. };
    double pt[N] = { other.x, other.y, other.z, 1. };
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            result[i] += this->mat_[i][j] * pt[j];
    return Point(result[0] / result[3], result[1] / result[3], result[2] / result[3]);
}

Trsf
Trsf::scaled(double factor)
{
    auto s = Trsf::identity();
    for (auto i = 0; i < N - 1; i++)
        s.mat_[i][i] = factor;
    return s;
}

Trsf
Trsf::scaled(double factor_x, double factor_y, double factor_z)
{
    auto s = Trsf::identity();
    s.mat_[0][0] = factor_x;
    s.mat_[1][1] = factor_y;
    s.mat_[2][2] = factor_z;
    return s;
}

Trsf
Trsf::translated(double tx, double ty, double tz)
{
    auto t = Trsf::identity();
    t.mat_[0][3] = tx;
    t.mat_[1][3] = ty;
    t.mat_[2][3] = tz;
    return t;
}

Trsf
Trsf::rotated_x(double theta)
{
    auto r = Trsf::identity();
    r.mat_[1][1] = std::cos(theta);
    r.mat_[1][2] = -std::sin(theta);
    r.mat_[2][1] = std::sin(theta);
    r.mat_[2][2] = std::cos(theta);
    return r;
}

Trsf
Trsf::rotated_y(double theta)
{
    auto r = Trsf::identity();
    r.mat_[0][0] = std::cos(theta);
    r.mat_[0][2] = std::sin(theta);
    r.mat_[2][0] = -std::sin(theta);
    r.mat_[2][2] = std::cos(theta);
    return r;
}

Trsf
Trsf::rotated_z(double theta)
{
    auto r = Trsf::identity();
    r.mat_[0][0] = std::cos(theta);
    r.mat_[0][1] = -std::sin(theta);
    r.mat_[1][0] = std::sin(theta);
    r.mat_[1][1] = std::cos(theta);
    return r;
}

Trsf
Trsf::identity()
{
    Trsf result;
    for (auto i = 0; i < N; i++)
        result.mat_[i][i] = 1.;
    return result;
}

} // namespace krado
