// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/geom_curve.h"
#include "krado/mesh_vertex_abstract.h"

namespace krado {

class MeshCurveVertex : public MeshVertexAbstract {
public:
    /// Construct vertex on a curve
    ///
    /// @param geom_curve Geometrical curve
    /// @param u Parameter on the curve
    MeshCurveVertex(const GeomCurve & geom_curve, double u);

    /// Get geometrical curve this vertex is connected to
    ///
    /// @return Geometrical curve this vertex is connected to
    [[nodiscard]] const GeomCurve & geom_curve() const;

    /// Get the parametrical position of the vertex on the curve
    ///
    /// @return Parametrical position of the vertex on the curve
    [[nodiscard]] double parameter() const;

    /// Get physical position in the 3D space
    ///
    /// @return Physical position in the 3D space
    [[nodiscard]] Point point() const override;

private:
    const GeomCurve & gcurve_;
    /// Parametrical position on the curve
    double u_;
    /// Physical location corresponding to the `u` parameter
    Point phys_pt_;
};

} // namespace krado
