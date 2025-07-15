// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/predicates.h"
#include "krado/point.h"
#include "krado/uv_param.h"
#include "robust_predicates/robust_predicates.h"

namespace krado {

double
insphere(const Point & pa, const Point & pb, const Point & pc, const Point & pd, const Point & pe)
{
    double da[3] = { pa.x, pa.y, pa.z };
    double db[3] = { pb.x, pb.y, pb.z };
    double dc[3] = { pc.x, pc.y, pc.z };
    double dd[3] = { pd.x, pd.y, pd.z };
    double de[3] = { pe.x, pe.y, pe.z };
    return robust_predicates::insphere(da, db, dc, dd, de);
}

double
orient3d(const Point & pa, const Point & pb, const Point & pc, const Point & pd)
{
    double da[3] = { pa.x, pa.y, pa.z };
    double db[3] = { pb.x, pb.y, pb.z };
    double dc[3] = { pc.x, pc.y, pc.z };
    double dd[3] = { pd.x, pd.y, pd.z };
    return robust_predicates::orient3d(da, db, dc, dd);
}

double
incircle(const Point & pa, const Point & pb, const Point & pc, const Point & pd)
{
    double da[2] = { pa.x, pa.y };
    double db[2] = { pb.x, pb.y };
    double dc[2] = { pc.x, pc.y };
    double dd[2] = { pd.x, pd.y };
    return robust_predicates::incircle(da, db, dc, dd);
}

double
incircle(const UVParam & pa, const UVParam & pb, const UVParam & pc, const UVParam & pd)
{
    double da[2] = { pa.u, pa.v };
    double db[2] = { pb.u, pb.v };
    double dc[2] = { pc.u, pc.v };
    double dd[2] = { pd.u, pd.v };
    return robust_predicates::incircle(da, db, dc, dd);
}

double
orient2d(const Point & pa, const Point & pb, const Point & pc)
{
    double da[2] = { pa.x, pa.y };
    double db[2] = { pb.x, pb.y };
    double dc[2] = { pc.x, pc.y };
    return robust_predicates::orient2d(da, db, dc);
}

double
orient2d(const UVParam & pa, const UVParam & pb, const UVParam & pc)
{
    double da[2] = { pa.u, pa.v };
    double db[2] = { pb.u, pb.v };
    double dc[2] = { pc.u, pc.v };
    return robust_predicates::orient2d(da, db, dc);
}

} // namespace krado
