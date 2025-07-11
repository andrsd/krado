// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/pattern.h"

namespace krado {

Pattern::Pattern() {}

Pattern::Pattern(const std::vector<Point> & points) : pts_(points) {}

const std::vector<Point> &
Pattern::points() const
{
    return this->pts_;
}

void
Pattern::set_points(std::vector<Point> && points)
{
    this->pts_ = std::move(points);
}

} // namespace krado
