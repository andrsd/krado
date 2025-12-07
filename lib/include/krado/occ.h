// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "gp_Pnt.hxx"
#include "gp_Dir.hxx"
#include "gp_Vec.hxx"
#include "gp_Ax1.hxx"
#include "gp_Ax2.hxx"
#include "gp_Circ.hxx"

namespace krado {

class Point;
class Vector;
class Axis1;
class Axis2;
class Circle;

namespace occ {

/// Convert krado Vector to OpenCASCADE gp_Dir
gp_Dir to_dir(const Vector & vec);

} // namespace occ
} // namespace krado
