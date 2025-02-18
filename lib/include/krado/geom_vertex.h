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

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Vertex vertex_;
    double x_, y_, z_;
};

} // namespace krado
