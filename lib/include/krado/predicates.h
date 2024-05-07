// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class UVParam;
class Point;

double orient2d(const UVParam & pa, const UVParam & pb, const UVParam & pc);
double orient3d(const Point & pa, const Point & pb, const Point & pc, const Point & pd);

} // namespace krado
