// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh.h"
#include "krado/vector.h"

namespace krado {

/// Extrude a mesh
///
/// @param mesh Mesh to extrude
/// @param direction Extrusion direction
/// @param layers Number of layers
/// @param thickness Thickness of the extrusion
/// @return Extruded mesh
Mesh extrude(const Mesh & mesh, const Vector & direction, int layers, double thickness);

/// Extrude a mesh
///
/// @param mesh Mesh to extrude
/// @param direction Extrusion direction
/// @param thicknesses Thickness of each layer
/// @return Extruded mesh
Mesh extrude(const Mesh & mesh, const Vector & direction, const std::vector<double> & thicknesses);

} // namespace krado
