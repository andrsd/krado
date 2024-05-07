#pragma once

#include "krado/geom_curve.h"

namespace krado {

class MeshCurveVertex {
public:
    /// Construct vertex on a curve
    ///
    /// @param geom_curve Geometrical curve
    /// @param u Parameter on the curve
    MeshCurveVertex(const GeomCurve & geom_curve, double u);

    /// Get geometrical curve this vertex is connected to
    ///
    /// @return Geometrical curve this vertex is connected to
    const GeomCurve & geom_curve() const;

    /// Get the parametrical position of the vertex on the curve
    ///
    /// @return Parametrical position of the vertex on the curve
    double parameter() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    Point point() const;

private:
    const GeomCurve & gcurve;
    /// Parametrical position on the curve
    double u;
    /// Physical location corresponding to the `u` parameter
    Point phys_pt;
};

} // namespace krado
