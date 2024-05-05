#pragma once

#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_point.h"
#include <vector>

namespace krado {

class Model;

class Mesh {
public:
    Mesh(const Model & model);

    /// Vertex
    const MeshVertex & vertex(int id) const;

    /// Vertices
    const std::vector<MeshVertex> & vertices() const;

    /// Curve
    const MeshCurve & curve(int id) const;

    /// Curves
    const std::vector<MeshCurve> & curves() const;

private:
    void initialize(const Model & model);

    std::vector<MeshVertex> vtxs;
    std::vector<MeshCurve> crvs;
};

} // namespace krado
