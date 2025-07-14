// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/uv_param.h"
// #include <Eigen/Eigen>

namespace krado {

class Vector;

/// Sign function
///
/// @tparam T C++ type
/// @param value Value to check
/// @return -1 for negative values, 0 for zero, and 1 for positive values
template <typename T>
inline int
sign(T const & value)
{
    return (T(0) < value) - (value < T(0));
}

UVParam circum_center_xy(const UVParam & p1, const UVParam & p2, const UVParam & p3);

Point circum_center_xyz(Point p1, Point p2, Point p3, std::tuple<double, double> * uv = nullptr);

double triangle_area(const Point & p0, const Point & p1, const Point & p2);

Vector normal3points(const Point & a, const Point & b, const Point & c);

#if 0
Eigen::Vector2d sys2x2(const Eigen::Matrix2d & mat, const Eigen::Vector2d & rhs);
#endif

int intersection_segments(const Point & p1,
                          const Point & p2,
                          const Point & q1,
                          const Point & q2,
                          UVParam & x);

int intersection_segments(const UVParam & p1,
                          const UVParam & p2,
                          const UVParam & q1,
                          const UVParam & q2,
                          UVParam & x);

#if 0
bool newton_fd(bool (*func)(Eigen::VectorXd &, Eigen::VectorXd &, void *),
               Eigen::VectorXd & x,
               void * data,
               double relax = 1.,
               double tolx = 1.e-6);
#endif

} // namespace krado
