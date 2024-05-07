// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_vertex_abstract.h"
#include "krado/point.h"
#include "krado/uv_param.h"

namespace krado {

class GeomSurface;
class UVParam;

class MeshSurfaceVertex : public MeshVertexAbstract {
public:
    /// Construct vertex on a curve
    ///
    /// @param geom_curve Geometrical curve
    /// @param u Parameter on the curve
    /// @param v Parameter on the curve
    MeshSurfaceVertex(const GeomSurface & geom_surface, double u, double v);

    MeshSurfaceVertex(const GeomSurface & geom_surface, UVParam uv);

    /// Get geometrical curve this vertex is connected to
    ///
    /// @return Geometrical curve this vertex is connected to
    [[nodiscard]] const GeomSurface & geom_surface() const;

    /// Get the parametrical position of the vertex on the curve
    ///
    /// @return Parametrical position of the vertex on the curve
    [[nodiscard]] UVParam parameter() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] Point point() const override;

private:
    const GeomSurface & gsurface_;
    /// Parametrical position on the curve
    UVParam uv_;
    /// Physical location corresponding to the (`u`, `v`) parameter
    Point phys_pt_;
};

} // namespace krado
