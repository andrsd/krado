// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/point.h"

namespace krado {

/// Box
class Box : public GeomVolume {
public:
    Box(const Point & pt1, const Point & pt2);
};

} // namespace krado
