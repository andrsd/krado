// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/axis1.h"

namespace krado {

Axis1::Axis1() : loc_ { 0.0, 0.0, 0.0 }, dir_ { 0.0, 0.0, 1.0 } {}

Axis1::Axis1(const Point & loc, const Vector & dir) : loc_(loc), dir_(dir.normalized()) {}

const Point &
Axis1::location() const
{
    return this->loc_;
}

const Vector &
Axis1::direction() const
{
    return this->dir_;
}

bool
Axis1::is_equal(const Axis1 & other, double tol) const
{
    return this->loc_.is_equal(other.loc_, tol) && (this->dir_ - other.dir_).magnitude() <= tol;
}

} // namespace krado
