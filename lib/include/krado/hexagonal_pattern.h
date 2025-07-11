// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/pattern.h"
#include "krado/axis2.h"

namespace krado {

/// Hexagonal pattern
class HexagonalPattern : public Pattern {
public:
    HexagonalPattern(const Axis2 & center, double flat_to_flat, int side_segs);

    /// Get flat to flat distance
    double flat_to_flat() const;

private:
    /// Center of the pattern
    Axis2 center_;
    /// Flat to flat distance
    double flat_to_flat_;
    /// Number of segments per side
    int num_side_segs_;
};

} // namespace krado
