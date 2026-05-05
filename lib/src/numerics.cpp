// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/numerics.h"

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

} // namespace krado
