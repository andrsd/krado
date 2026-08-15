// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/point.h"
#include "TopoDS_Solid.hxx"
#include <vector>

namespace krado {

class GeomSurface;
class GeomModel;

class GeomVolume : public GeomShape {
public:
    explicit GeomVolume(const TopoDS_Solid & solid);

    [[nodiscard]] int dim() const final;

    /// Get surfaces bounding this volume
    ///
    /// @return Curves bounding the surface
    [[nodiscard]] std::vector<GeomSurface> surfaces() const;

    /// Get mesh size at parameter (interpolates between mesh size at bounding vertices)
    ///
    /// @param u Parameter value
    /// @return Mesh size at parameter
    [[nodiscard]] double mesh_size_at_param(Point pos) const;

    operator const TopoDS_Solid &() const;

public:
    static GeomVolume create(const GeomShape & shape);
};

} // namespace krado
