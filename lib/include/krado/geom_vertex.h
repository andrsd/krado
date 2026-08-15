// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/point.h"
#include "TopoDS_Vertex.hxx"

namespace krado {

class GeomModel;

class GeomVertex : public GeomShape {
public:
    explicit GeomVertex(const TopoDS_Vertex & vertex);

    int dim() const final;

    /// Is this a null vertex.  Infinite curves report null vertices as their bounding vertices.
    ///
    /// @return `true` if this is null vertex, `false` otherwise
    [[nodiscard]] bool is_null() const;

    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
    [[nodiscard]] double z() const;

    /// Get vertex location as a point
    ///
    /// @return Vertex location
    [[nodiscard]] Point point() const;

    /// Get the mesh size at the vertex.
    ///
    /// @return The mesh size at the vertex.
    double mesh_size() const;

    operator const TopoDS_Vertex &() const;

    bool operator<(const GeomVertex & other) const;

private:
    /// Physical location of the point
    Point pt_;
    /// Mesh size at the vertex.
    double mesh_size_;

    friend class MeshVertex;
};

} // namespace krado

std::ostream & operator<<(std::ostream & stream, const krado::GeomVertex & vtx);
