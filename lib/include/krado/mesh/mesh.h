#pragma once

#include "krado/mesh/vertex.h"
#include "krado/mesh/curve.h"
#include "krado/mesh/point.h"
#include "krado/mesh/line2.h"
#include <vector>

namespace krado::mesh {

class Mesh {
public:
    Mesh();

    /// Vertices
    const std::vector<Vertex *> & vertices() const;

    /// Curves
    const std::vector<Curve *> & curves() const;

    /// Mesh points
    const std::vector<Point *> & points() const;

    /// Lines
    const std::vector<Line2 *> & lines() const;

private:
    std::vector<Vertex *> vtxs;
    std::vector<Curve *> crvs;
    std::vector<Point *> pts;
    std::vector<Line2 *> lns;
};

} // namespace krado::mesh
