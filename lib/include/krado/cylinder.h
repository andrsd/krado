// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/axis2.h"

namespace krado {

class Cylinder : public GeomVolume {
private:
    Cylinder(const TopoDS_Solid & solid);

public:
    /// Create a cylinder
    ///
    /// @param location Location in space
    /// @param radius Radius
    /// @param height Height
    static Cylinder create(const Axis2 & location, double radius, double height);
};

} // namespace krado
