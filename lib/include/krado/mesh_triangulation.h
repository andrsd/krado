#pragma once

#include "krado/poly_mesh.h"

namespace krado {

class MeshSurface;

/// Returns a half edge data structure that is actually the triangulation face boundary
/// - if recover = true, edges are recovered and the triangulation is colored: triangles belonging
///   to the model face are colored faceTag, other have negative colors.
PolyMesh surface_initial_mesh(MeshSurface & msurface,
                              bool recover = false,
                              std::vector<double> * additional = nullptr);

} // namespace krado
