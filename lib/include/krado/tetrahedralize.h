// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"

namespace krado {

class Mesh;

/// Tetrahedralize a mesh
///
/// @param mesh Mesh to tetrahedralize
/// @return Tetrahedral mesh
[[nodiscard]] Ptr<Mesh> tetrahedralize(Ptr<const Mesh> mesh);

} // namespace krado
