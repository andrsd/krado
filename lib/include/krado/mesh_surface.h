#pragma once

#include "krado/geom_surface.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshCurve;

class MeshSurface : public MeshingParameters {
public:
    MeshSurface(const GeomSurface & gcurve, const std::vector<MeshCurve *> & mesh_curves);

    /// Get geometrical surface associated with this surface
    ///
    /// @return Geometrical surface associated with this surface
    const GeomSurface & geom_surface() const;

    /// Get curves bounding this surface
    const std::vector<MeshCurve *> & curves() const;

private:
    const GeomSurface & gsurface;
    /// Mesh curves bounding this surface
    std::vector<MeshCurve *> mesh_curves;
};

} // namespace krado
