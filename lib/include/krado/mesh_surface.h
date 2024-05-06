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

    /// Check if the surface is already meshed
    ///
    /// @return `true` if mesh is already present, `false` otherwise
    bool is_meshed() const;

    /// Mark surface as meshed
    void set_meshed();

private:
    const GeomSurface & gsurface;
    /// Mesh curves bounding this surface
    std::vector<MeshCurve *> mesh_curves;
    /// Flag indicating if the surface is meshed
    bool meshed;
};

} // namespace krado
