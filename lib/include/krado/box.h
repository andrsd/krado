// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_volume.h"

namespace krado {

class Point;

/// Box
class Box : public GeomVolume {
private:
    Box(const TopoDS_Solid & solid);

public:
    static Box create(Point pt1, Point pt2);
};

} // namespace krado
