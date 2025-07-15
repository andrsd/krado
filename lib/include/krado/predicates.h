// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class UVParam;
class Point;

double
insphere(const Point & pa, const Point & pb, const Point & pc, const Point & pd, const Point & pe);
double orient3d(const Point & pa, const Point & pb, const Point & pc, const Point & pd);
double incircle(const Point & pa, const Point & pb, const Point & pc, const Point & pd);
double incircle(const UVParam & pa, const UVParam & pb, const UVParam & pc, const UVParam & pd);
double orient2d(const Point & pa, const Point & pb, const Point & pc);
double orient2d(const UVParam & pa, const UVParam & pb, const UVParam & pc);

} // namespace krado
