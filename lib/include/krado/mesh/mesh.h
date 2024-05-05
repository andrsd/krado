#pragma once

#include "krado/mesh/vertex.h"
#include "krado/mesh/curve.h"
#include "krado/mesh/point.h"
#include "krado/mesh/line2.h"
#include <vector>

namespace krado::geo {
class Model;
}

namespace krado::mesh {

class Mesh {
public:
    Mesh(const geo::Model & model);

    /// Vertex
    const Vertex & vertex(int id) const;

    /// Vertices
    const std::vector<Vertex> & vertices() const;

    /// Curve
    const Curve & curve(int id) const;

    /// Curves
    const std::vector<Curve> & curves() const;

private:
    void initialize(const geo::Model & model);

    std::vector<Vertex> vtxs;
    std::vector<Curve> crvs;
};

} // namespace krado::mesh
