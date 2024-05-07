// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/predicates.h"
#include "krado/robust_predicates.h"
#include "krado/point.h"
#include "krado/uv_param.h"

namespace krado {

double
orient2d(const UVParam & pa, const UVParam & pb, const UVParam & pc)
{
    double da[2] = { pa.u, pa.v };
    double db[2] = { pb.u, pb.v };
    double dc[2] = { pc.u, pc.v };
    return robust_predicates::orient2d(da, db, dc);
}

double
orient3d(const Point & pa, const Point & pb, const Point & pc, const Point & pd)
{
    double p0[3] = { pa.x, pa.y, pa.z };
    double p1[3] = { pb.x, pb.y, pb.z };
    double p2[3] = { pc.x, pc.y, pc.z };
    double p3[3] = { pd.x, pd.y, pd.z };
    return robust_predicates::orient3d(p0, p1, p2, p3);
}

} // namespace krado
