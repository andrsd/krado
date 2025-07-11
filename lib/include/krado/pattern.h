// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include <vector>

namespace krado {

/// Base class for patterns
class Pattern {
public:
    Pattern();
    Pattern(const std::vector<Point> & points);

    ///
    const std::vector<Point> & points() const;

protected:
    void set_points(std::vector<Point> && points);

private:
    std::vector<Point> pts_;
};

} // namespace krado
