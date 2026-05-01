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

    /// Geometrical shape associated with this vertex
    ///
    /// @return Geometrical shape associated with this vertex
    const GeomShape & geom_shape() const;

private:
    /// Geometrical shape associated with this vertex
    const GeomShape & geom_shape_;
};

} // namespace krado
