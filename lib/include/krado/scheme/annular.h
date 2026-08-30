// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/ptr.h"
#include "krado/scheme.h"
#include "krado/scheme2d.h"
#include "krado/mesh_vertex_abstract.h"
#include <vector>

namespace krado {

class MeshSurface;
class MeshCurve;
class Point;

class SchemeAnnular : public Scheme, public Scheme2D {
public:
    SchemeAnnular(const std::string & name);
};

/// Compute average distance from points in a loop to a center
///
/// @param loop "Loop" with vertex points
/// @param ctr_pnt Center point
/// @return Average distance
double avg_distance(const std::vector<Ptr<MeshVertexAbstract>> & loop, Point ctr_pnt);

/// Find "center" point of surface
///
/// @param mesh_surface Mesh surface to find center for (used if it is bounded by a circular curve)
/// @param loops Vertex "loops" that define the boundary
/// @return Center point
Point find_center_point(Ptr<MeshSurface> mesh_surface,
                        const std::vector<std::vector<Ptr<MeshVertexAbstract>>> & loops);

std::vector<std::vector<Ptr<MeshVertexAbstract>>> get_boundary_loops(Ptr<MeshSurface> surface);

Point interpolate_loop(const std::vector<Ptr<MeshVertexAbstract>> & loop,
                       const std::vector<double> & L,
                       double l);

std::vector<double> get_L(const std::vector<Ptr<MeshVertexAbstract>> & loop);

} // namespace krado
