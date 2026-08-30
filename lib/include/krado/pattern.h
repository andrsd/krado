// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include <vector>

namespace krado {

/// Base class for patterns
class Pattern {
protected:
    Pattern(const std::vector<Point> & points);

public:
    ///
    [[nodiscard]] const std::vector<Point> & points() const;

private:
    std::vector<Point> pts_;
};

} // namespace krado
