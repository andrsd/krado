#pragma once

#include "krado/geo/vertex.h"

namespace krado::mesh {

class Mesh;

class Vertex {
public:
    Vertex(const geo::Vertex & geom_vertex);

    const geo::Vertex & geom_vertex() const;

private:
    const geo::Vertex & gvtx;
};

} // namespace krado::mesh
