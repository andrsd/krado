// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <tuple>

namespace krado {

class GeomShape;
class Point;
class GeomCurve;
class GeomSurface;
class UVParam;

class MeshVertexAbstract {
public:
    MeshVertexAbstract(const GeomShape & geom_shape);
    virtual ~MeshVertexAbstract() = default;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] virtual Point point() const = 0;

    /// Get global ID
    ///
    /// @return Global ID of this vertex
    [[nodiscard]] int global_id() const;

    /// Set global ID
    ///
    /// @param id New ID to assign
    void set_global_id(int id);

    /// Geometrical shape associated with this vertex
    ///
    /// @return Geometrical shape associated with this vertex
    const GeomShape & geom_shape() const;

private:
    /// Global ID of this vertex
    int gid_;
    /// Geometrical shape associated with this vertex
    const GeomShape & geom_shape_;
    int num_;
};

} // namespace krado
