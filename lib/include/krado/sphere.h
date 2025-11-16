// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/point.h"

namespace krado {

class Sphere : public GeomVolume {
public:
    /// Create a sphere
    ///
    /// @param center Location of the sphere center
    /// @param radius Radius of the sphere
    Sphere(const Point & center, double radius);
};

} // namespace krado
