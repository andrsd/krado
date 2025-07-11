// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/pattern.h"

namespace krado {

Pattern::Pattern() {}

const std::vector<Point> &
Pattern::points() const
{
    return this->pts_;
}

void
Pattern::set_points(const std::vector<Point> & points)
{
    this->pts_ = points;
}

} // namespace krado
