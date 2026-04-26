// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/point.h"
#include "krado/ptr.h"
#include <map>

namespace krado {

class MeshVertexAbstract;
class MeshSurface;

/// Maps local indices produced by a mesh generators to the mesh surface
class SurfaceIndexMapper {
public:
    SurfaceIndexMapper(Ptr<MeshSurface> surface);
    ~SurfaceIndexMapper();

    [[nodiscard]] Ptr<MeshVertexAbstract> surface_vertex(double x, double y);

    [[nodiscard]] Ptr<MeshVertexAbstract> curve_vertex(int curve_idx, double x, double y);

private:
    Ptr<MeshSurface> surface_;
    /// Map from physical location to local surface vertex index
    std::map<Point, Ptr<MeshVertexAbstract>> surf_idx_;
    /// Map from physical location to local curve vertex index
    std::map<int, std::map<Point, Ptr<MeshVertexAbstract>>> curv_surf_idx_;
};

} // namespace krado
