// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/types.h"
#include "krado/point.h"
#include <vector>

namespace krado {

/// Quadrature point
struct QuadraturePoint {
    /// Coordinate in reference space
    Point point;
    /// Weight
    double weight;
};

/// Quadrature rules
class Quadrature {
public:
    /// Get quadrature rule for an element type
    ///
    /// @param type Element type
    /// @return Quadrature points and weights
    static std::vector<QuadraturePoint> get(ElementType type);
};

} // namespace krado
