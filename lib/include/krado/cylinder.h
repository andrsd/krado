// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/axis2.h"

namespace krado {

class Cylinder : public GeomVolume {
public:
    /// Create a cylinder
    ///
    /// @param location Location in space
    /// @param radius Radius
    /// @param height Height
    Cylinder(const Axis2 & location, double radius, double height);
};

} // namespace krado
