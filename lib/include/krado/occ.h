// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gp_Pnt.hxx"
#include "gp_Dir.hxx"
#include "gp_Vec.hxx"
#include "gp_Ax1.hxx"
#include "gp_Ax2.hxx"

namespace krado {

class Point;
class Vector;
class Axis1;
class Axis2;

namespace occ {

/// Convert krado Point to OpenCASCADE gp_Pnt
gp_Pnt to_pnt(const Point & pt);

/// Convert krado Vector to OpenCASCADE gp_Vec
gp_Vec to_vec(const Vector & vec);

/// Convert krado Vector to OpenCASCADE gp_Dir
gp_Dir to_dir(const Vector & vec);

/// Convert krado Axis1 to OpenCASCADE gp_Ax1
gp_Ax1 to_ax1(const Axis1 & ax1);

/// Convert krado Axis1 to OpenCASCADE gp_Ax1
gp_Ax2 to_ax2(const Axis2 & ax2);

} // namespace occ
} // namespace krado
