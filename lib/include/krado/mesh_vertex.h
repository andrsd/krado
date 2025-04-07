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

    /// Get the unique identifier of the vertex.
    ///
    /// @return The unique identifier of the vertex.
    int id() const;

    /// Get geometrical vertex associated with this vertex
    ///
    /// @return Geometrical vertex associated with this vertex
    [[nodiscard]] const GeomVertex & geom_vertex() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] Point point() const override;

    /// Get the mesh size at the vertex.
    ///
    /// @return The mesh size at the vertex.
    double mesh_size() const;

    /// Set the mesh size at the vertex.
    ///
    /// @param size The new mesh size.
    void set_mesh_size(double size);

private:
    const GeomVertex & gvtx_;
    /// Mesh size at the vertex.
    double mesh_size_;

public:
    struct PtrLessThan {
        bool
        operator()(const MeshVertex * v1, const MeshVertex * v2) const
        {
            return v1->id() < v2->id();
        }
    };
};

} // namespace krado
