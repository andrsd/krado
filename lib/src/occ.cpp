// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/occ.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "krado/axis1.h"
#include "krado/axis2.h"
#include "krado/circle.h"

namespace krado {
namespace occ {

gp_Pnt
to_pnt(const Point & pt)
{
    return gp_Pnt(pt.x, pt.y, pt.z);
}

gp_Vec
to_vec(const Vector & vec)
{
    return gp_Vec(vec.x, vec.y, vec.z);
}

gp_Dir
to_dir(const Vector & vec)
{
    auto dir = vec.normalized();
    return gp_Dir(dir.x, dir.y, dir.z);
}

gp_Ax1
to_ax1(const Axis1 & ax1)
{
    return gp_Ax1(to_pnt(ax1.location()), to_dir(ax1.direction()));
}

gp_Ax2
to_ax2(const Axis2 & ax2)
{
    return gp_Ax2(to_pnt(ax2.location()), to_dir(ax2.direction()));
}

gp_Circ
to_circ(const Circle & circle)
{
    return circle.circ_;
}

} // namespace occ
} // namespace krado
