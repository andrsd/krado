#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_vertex.h"

namespace krado {

class Mesh;

class MeshCurve {
public:
    MeshCurve(const GeomCurve & gcurve, const MeshVertex * v1, const MeshVertex * v2);

    const GeomCurve & geom_curve() const;

    /// Get curve marker
    ///
    /// @return Curve marker
    int marker() const;

    /// Set curve marker
    ///
    /// @param marker New curve marker
    void set_marker(int marker);

private:
    const GeomCurve & gcurve;
    const MeshVertex * v1;
    const MeshVertex * v2;
    /// Curve marker
    int curve_marker;
};

} // namespace krado
