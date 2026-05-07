// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/numerics.h"
#include "krado/point.h"
#include "krado/uv_param.h"
#include "krado/vector.h"

namespace krado {

Optional<std::array<double, 2>>
sys2x2(std::array<std::array<double, 2>, 2> mat, std::array<double, 2> b)
{
    const auto norm = std::pow(mat[0][0], 2) + std::pow(mat[1][1], 2) + std::pow(mat[0][1], 2) +
                      std::pow(mat[1][0], 2);

    const auto det = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];

    // Check for singular matrix
    if (norm == 0.0 || std::abs(det) / norm < 1.e-16)
        return std::nullopt;
    const auto ud = 1.0 / det;

    std::array<double, 2> res;
    res[0] = b[0] * mat[1][1] - mat[0][1] * b[1];
    res[1] = mat[0][0] * b[1] - mat[1][0] * b[0];

    for (int i = 0; i < 2; i++)
        res[i] *= ud;

    return { res };
}

Vector
normal(Point p0, Point p1, Point p2)
{
    auto t1 = p0 - p1;
    auto t2 = p2 - p0;
    auto n = cross_product(t1, t2);
    return n.normalized();
}

Vector
normal(UVParam uv0, UVParam uv1, UVParam uv2)
{
    return normal(Point(uv0.u, uv0.v, 0.), Point(uv1.u, uv1.v, 0.), Point(uv2.u, uv2.v, 0.));
}

} // namespace krado
