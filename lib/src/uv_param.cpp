// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/uv_param.h"
#include "krado/exception.h"
#include "krado/point.h"

namespace krado {

UVParam::UVParam() : u(0.), v(0.) {}

UVParam::UVParam(double u, double v) : u(u), v(v) {}

UVParam &
UVParam::operator+=(const UVParam & a)
{
    this->u += a.u;
    this->v += a.v;
    return *this;
}

UVParam &
UVParam::operator/=(double alpha)
{
    this->u /= alpha;
    this->v /= alpha;
    return *this;
}

} // namespace krado
