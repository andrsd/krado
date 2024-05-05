#pragma once

#include "krado/geom_surface.h"

namespace krado {

class MeshSurface {
public:
    MeshSurface(const GeomSurface & gcurve);

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

private:
    const GeomSurface & gsurface;
    /// Surface marker
    int surface_marker;
};

} // namespace krado
