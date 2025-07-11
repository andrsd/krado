// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/vector.h"

namespace krado {

class Axis2 {
public:
    /// Default constructor: (0,0,0) with XYZ basis
    Axis2();

    /// Constructor: origin + main direction
    ///
    /// @param P Origin point
    /// @param V Main direction vector
    Axis2(const Point & P, const Vector & V);

    /// Constructor: origin + normal + X-like vector (like gp_Ax2(P, N, Vx))
    ///
    /// @param P Origin point
    /// @param N Normal vector
    /// @param Vx X-like vector
    Axis2(const Point & P, const Vector & N, const Vector & Vx);

    const Point & location() const;

    const Vector & x_direction() const;

    const Vector & y_direction() const;

    const Vector & z_direction() const;

    /// Transform local (u,v) coords in XY plane to 3D point
    Point local_to_world(double u, double v) const;

    /// (Optional) World → local in Axis2 frame
    Vector world_to_local(const Point & pt) const;

private:
    Point loc_;
    Vector x_dir_;
    Vector y_dir_;
    Vector z_dir_;
};

} // namespace krado
