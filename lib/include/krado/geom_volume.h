#pragma once

#include "krado/geom_surface.h"
#include "TopoDS_Solid.hxx"
#include <vector>

namespace krado {

class Model;

class GeomVolume {
public:
    explicit GeomVolume(const TopoDS_Solid & solid);

    /// Get volume of the volume
    ///
    /// @return Volume of the volume
    double volume() const;

    /// Get surfaces bounding this volume
    ///
    /// @return Curves bounding the surface
    std::vector<GeomSurface> surfaces() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Solid solid;
    /// volume
    double vol;
};

} // namespace krado
