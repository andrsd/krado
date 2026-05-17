// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"

namespace krado {

class Point;

class Sphere : public GeomVolume {
private:
    explicit Sphere(const TopoDS_Solid & solid);

public:
    /// Create a sphere
    ///
    /// @param center Location of the sphere center
    /// @param radius Radius of the sphere
    static Sphere create(Point center, double radius);
};

} // namespace krado
