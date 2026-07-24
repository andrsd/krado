// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/quadrature.h"
#include "krado/element.h"
#include "krado/exception.h"
#include <cmath>

namespace krado {

std::vector<QuadraturePoint>
Quadrature::get(ElementType type)
{
    switch (type) {
    case ElementType::LINE2:
        return { { Point(0, 0, 0), 2.0 } };

    case ElementType::TRI3:
        return { { Point(1. / 3., 1. / 3., 0.), 0.5 } };

    case ElementType::QUAD4:
        return { { Point(0, 0, 0), 4.0 } };

    case ElementType::TETRA4:
        return { { Point(0.25, 0.25, 0.25), 1. / 6. } };

    case ElementType::HEX8:
        return { { Point(0, 0, 0), 8.0 } };

    case ElementType::PRISM6:
        return { { Point(1. / 3., 1. / 3., 0.), 1.0 } };

    case ElementType::PYRAMID5:
        return { { Point(0, 0, 1.0 - 1.0 / std::sqrt(3.0)), 4.0 } };

    default:
        throw Exception("Quadrature not implemented for element type {}", type);
    }
}

} // namespace krado
