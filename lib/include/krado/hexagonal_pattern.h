// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/pattern.h"
#include "krado/axis2.h"

namespace krado {

/// Hexagonal pattern
class HexagonalPattern : public Pattern {
    HexagonalPattern(const std::vector<Point> & points,
                     const Axis2 & center,
                     double flat_to_flat,
                     int side_segs);

public:
    [[nodiscard]] Point center() const;

    /// Get flat to flat distance
    [[nodiscard]] double flat_to_flat() const;

    ///
    [[nodiscard]] int num_side_segments() const;

private:
    /// Center of the pattern
    Axis2 center_;
    /// Flat to flat distance
    double flat_to_flat_;
    /// Number of segments per side
    int num_side_segs_;

public:
    /// Create hexagonal pattern
    ///
    /// @param center Center of the pattern
    /// @param flat_to_flat Distance from one flat to opposite flat surface
    /// @param side_segs Number of segment on a side
    /// @return Hexagonal pattern
    static HexagonalPattern create(const Axis2 & center, double flat_to_flat, int side_segs);
};

} // namespace krado
