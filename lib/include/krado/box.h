// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"
#include "krado/point.h"

namespace krado {

/// Box
class Box : public GeomVolume {
private:
    Box(const TopoDS_Solid & solid);

public:
    static Box create(const Point & pt1, const Point & pt2);
};

} // namespace krado
