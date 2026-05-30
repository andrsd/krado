// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class GeomShape;
class Point;

class MeshVertexAbstract {
public:
    MeshVertexAbstract(const GeomShape & geom_shape);
    virtual ~MeshVertexAbstract() = default;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] virtual Point point() const = 0;

    /// Relocate the vertex to a new position. The actual position will be projected
    /// onto the geometrical shape associated with this vertex.
    ///
    /// @param p New physical position
    virtual void relocate(const Point & p) = 0;

    /// Geometrical shape associated with this vertex
    ///
    /// @return Geometrical shape associated with this vertex
    const GeomShape & geom_shape() const;

    /// Get vertex number (this is globally unique number)
    ///
    /// This is/can be used by meshing algorithms that require vertex numbering
    ///
    /// @return Global vertex number
    int num() const;

private:
    /// Geometrical shape associated with this vertex
    const GeomShape & geom_shape_;
    /// vertex number
    int num_;
};

} // namespace krado
