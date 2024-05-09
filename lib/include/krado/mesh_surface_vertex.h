#pragma once

#include "krado/mesh_vertex_abstract.h"

namespace krado {

class GeomSurface;

class MeshSurfaceVertex : public MeshVertexAbstract {
public:
    /// Construct vertex on a curve
    ///
    /// @param geom_curve Geometrical curve
    /// @param u Parameter on the curve
    /// @param v Parameter on the curve
    MeshSurfaceVertex(const GeomSurface & geom_surface, double u, double v);

    /// Get geometrical curve this vertex is connected to
    ///
    /// @return Geometrical curve this vertex is connected to
    const GeomSurface & geom_surface() const;

    /// Get the parametrical position of the vertex on the curve
    ///
    /// @return Parametrical position of the vertex on the curve
    std::tuple<double, double> parameter() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    Point point() const override;

private:
    const GeomSurface & gsurface;
    /// Parametrical position on the curve
    double u, v;
    /// Physical location corresponding to the (`u`, `v`) parameter
    Point phys_pt;
};

} // namespace krado
