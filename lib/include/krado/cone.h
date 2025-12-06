// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/axis2.h"

namespace krado {

class Cone : public GeomVolume {
private:
    Cone(const TopoDS_Solid & solid);

public:
    /// Create a cone
    ///
    /// @param location Location in space
    /// @param radius1 First radius
    /// @param radius2 Second radius
    /// @param height Height
    static Cone create(const Axis2 & location, double radius1, double radius2, double height);
};

} // namespace krado
