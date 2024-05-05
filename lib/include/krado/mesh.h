#pragma once

#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_point.h"
#include "krado/line2.h"
#include <vector>

namespace krado::geo {
class Model;
}

namespace krado::mesh {

class Mesh {
public:
    Mesh(const geo::Model & model);

    /// Vertex
    const MeshVertex & vertex(int id) const;

    /// Vertices
    const std::vector<MeshVertex> & vertices() const;

    /// Curve
    const MeshCurve & curve(int id) const;

    /// Curves
    const std::vector<MeshCurve> & curves() const;

private:
    void initialize(const geo::Model & model);

    std::vector<MeshVertex> vtxs;
    std::vector<MeshCurve> crvs;
};

} // namespace krado::mesh
