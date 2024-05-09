// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "TopoDS_Vertex.hxx"

namespace krado {

class GeomModel;

class GeomVertex {
public:
    explicit GeomVertex(const TopoDS_Vertex & vertex);

    /// Is this a null vertex.  Infinite curves report null vertices as their bounding vertices.
    ///
    /// @return `true` if this is null vertex, `false` otherwise
    bool is_null() const;

    double x() const;
    double y() const;
    double z() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Vertex vertex;
    double x_coord, y_coord, z_coord;
};

} // namespace krado
