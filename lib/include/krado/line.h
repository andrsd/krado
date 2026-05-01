// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"

namespace krado {

class Point;

class Line : public GeomCurve {
private:
    Line(const TopoDS_Edge & curve);

public:
    static Line create(const Point & pt1, const Point & pt2);
};

} // namespace krado
