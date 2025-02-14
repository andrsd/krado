// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh.h"

namespace krado {

/// Tetrahedralize a mesh
///
/// @param mesh Mesh to tetrahedralize
/// @return Tetrahedral mesh
[[nodiscard]] Mesh tetrahedralize(const Mesh & mesh);

} // namespace krado
