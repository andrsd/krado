// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_vertex_abstract.h"
#include "krado/meshing_parameters.h"

namespace krado {

class GeomEntity;
class GeomVertex;
class Point;

class MeshVertex : public MeshVertexAbstract, public MeshingParameters {
public:
    MeshVertex(const GeomVertex & geom_vertex);

    int tag() const;

    /// Get geometrical vertex associated with this vertex
    ///
    /// @return Geometrical vertex associated with this vertex
    [[nodiscard]] const GeomVertex & geom_vertex() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] Point point() const override;

    ///
    double prescribed_mesh_size_at_vertex() const;

private:
    const GeomVertex & gvtx_;

public:
    struct PtrLessThan {
        bool
        operator()(const MeshVertex * v1, const MeshVertex * v2) const
        {
            return v1->tag() < v2->tag();
        }
    };
};

} // namespace krado
