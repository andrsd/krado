// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_shape.h"
#include "krado/geom_surface.h"
#include "TopoDS_Solid.hxx"
#include <vector>

namespace krado {

class GeomSurface;
class GeomModel;

class GeomVolume : public GeomShape {
public:
    explicit GeomVolume(const TopoDS_Solid & solid);

    /// Get volume of the volume
    ///
    /// @return Volume of the volume
    [[nodiscard]] double volume() const;

    /// Get surfaces bounding this volume
    ///
    /// @return Curves bounding the surface
    [[nodiscard]] std::vector<GeomSurface> surfaces() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Solid solid_;
    /// volume
    double volume_;
};

} // namespace krado
