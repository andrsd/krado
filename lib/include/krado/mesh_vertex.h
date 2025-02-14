// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/meshing_parameters.h"
#include "krado/point.h"

namespace krado {

class MeshVertex : public MeshVertexAbstract, public MeshingParameters {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    /// Get geometrical vertex associated with this vertex
    ///
    /// @return Geometrical vertex associated with this vertex
    [[nodiscard]] const GeomVertex & geom_vertex() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] Point point() const override;

private:
    const GeomVertex & gvtx;
};

} // namespace krado
