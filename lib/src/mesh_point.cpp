// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_point.h"
#include "krado/point.h"

namespace krado {

MeshPoint::MeshPoint(double x, double y, double z) : x(x), y(y), z(z) {}

MeshPoint::MeshPoint(const Point & pt) : x(pt.x), y(pt.y), z(pt.z) {}

} // namespace krado
