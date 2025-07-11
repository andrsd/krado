// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/axis2.h"
#include "krado/vector.h"
#include "krado/exception.h"

namespace krado {

Axis2::Axis2() :
    loc_ { 0.0, 0.0, 0.0 },
    x_dir_ { 1.0, 0.0, 0.0 },
    y_dir_ { 0.0, 1.0, 0.0 },
    z_dir_ { 0.0, 0.0, 1.0 }
{
}

Axis2::Axis2(const Point & pt, const Vector & v) : loc_ { pt }
{
    this->z_dir_ = v.normalized();

    auto ref = (std::abs(this->z_dir_.x) < 0.9) ? Vector { 1, 0, 0 } : Vector { 0, 1, 0 };

    this->x_dir_ = cross_product(this->z_dir_, ref).normalized();
    this->y_dir_ = cross_product(this->z_dir_, this->x_dir_).normalized();
}

Axis2::Axis2(const Point & pt, const Vector & n, const Vector & v_x) : loc_ { pt }
{
    this->z_dir_ = n.normalized();

    auto temp = cross_product(this->z_dir_, v_x);
    if (temp.magnitude() < 1e-9)
        throw Exception("Axis2: N and Vx are parallel");

    this->x_dir_ = cross_product(temp, this->z_dir_).normalized(); // (N ^ Vx) ^ N
    this->y_dir_ = cross_product(this->z_dir_, this->x_dir_).normalized();
}

const Point &
Axis2::location() const
{
    return this->loc_;
}

const Vector &
Axis2::x_direction() const
{
    return this->x_dir_;
}

const Vector &
Axis2::y_direction() const
{
    return this->y_dir_;
}

const Vector &
Axis2::direction() const
{
    return this->z_dir_;
}

Axis1
Axis2::axis() const
{
    return Axis1(this->loc_, this->z_dir_);
}

Point
Axis2::local_to_world(double u, double v) const
{
    return this->loc_ + this->x_dir_ * u + this->y_dir_ * v;
}

Vector
Axis2::world_to_local(const Point & pt) const
{
    auto rel = pt - this->loc_;
    return Vector { dot_product(rel, this->x_dir_),
                    dot_product(rel, this->y_dir_),
                    dot_product(rel, this->z_dir_) };
}

} // namespace krado
