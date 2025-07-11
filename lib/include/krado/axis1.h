// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/vector.h"

namespace krado {

class Axis1 {
public:
    Axis1();
    Axis1(const Point & loc, const Vector & dir);

    const Point & location() const;

    const Vector & direction() const;

    bool is_equal(const Axis1 & other, double tol) const;

private:
    Point loc_;
    Vector dir_;
};

} // namespace krado
