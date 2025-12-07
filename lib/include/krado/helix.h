// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "Standard_Handle.hxx"
#include "Geom_BSplineCurve.hxx"

namespace krado {

class Axis2;

class Helix : public GeomCurve {
private:
    Helix(const TopoDS_Edge & edge, Handle(Geom_BSplineCurve) spline);

private:
    Handle(Geom_BSplineCurve) spline_;

public:
    /// Construct a helix approximated by B-spline
    ///
    /// @param ax2 Center of the helix
    /// @param radius Helix radius
    /// @param height Helix height
    /// @param turns Number of turns
    /// @param start_angle Starting angle
    static Helix
    create(const Axis2 & ax2, double radius, double height, double turns, double start_angle = 0.);
};

} // namespace krado
