// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "krado/point.h"
#include "krado/vector.h"
#include "Standard_Handle.hxx"
#include "Geom_BSplineCurve.hxx"
#include <vector>

namespace krado {

class Spline : public GeomCurve {
private:
    Spline(const TopoDS_Edge & edge, Handle(Geom_BSplineCurve) spline);

    Handle(Geom_BSplineCurve) spline_;

public:
    /// Construct a B-spline that is passing through an array of points. The continuity will be C2.
    ///
    /// @param points Array of points
    static Spline create(const std::vector<Point> & points);

    /// Construct a B-spline that is passing through an array of points with specified tangency.
    /// The continuity will be C1.
    ///
    /// @param points Array of points
    /// @param initial_tg Initial tangent
    /// @param final_tg Final tangent
    static Spline
    create(const std::vector<Point> & points, const Vector & initial_tg, const Vector & final_tg);
};

} // namespace krado
