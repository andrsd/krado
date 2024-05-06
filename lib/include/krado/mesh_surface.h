#pragma once

#include "krado/geom_surface.h"
#include "krado/size_parameters.h"
#include "krado/meshing_parameters.h"

namespace krado {

class MeshCurve;

class MeshSurface : public SizeParameters, public MeshingParameters {
public:
    MeshSurface(const GeomSurface & gcurve, const std::vector<const MeshCurve *> & mesh_curves);

    /// Get geometrical surface associated with this surface
    ///
    /// @return Geometrical surface associated with this surface
    const GeomSurface & geom_surface() const;

    /// Get surface marker
    ///
    /// @return Surface marker
    int marker() const;

    /// Set surface marker
    ///
    /// @param marker New surface marker
    void set_marker(int marker);

    /// Get curves bounding this surface
    const std::vector<const MeshCurve *> & curves() const;

private:
    const GeomSurface & gsurface;
    /// Surface marker
    int surface_marker;
    /// Mesh curves bounding this surface
    std::vector<const MeshCurve *> mesh_curves;
};

} // namespace krado
