#pragma once

#include "krado/mesh/vertex.h"

namespace krado::mesh {

class Curve {
public:
    Curve(Vertex * v1, Vertex * v2);

private:
    Vertex *v1, *v2;
};

} // namespace krado::mesh
