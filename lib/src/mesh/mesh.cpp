#include "krado/mesh/mesh.h"

namespace krado::mesh {

Mesh::Mesh() {}

const std::vector<Vertex *> &
Mesh::vertices() const
{
    return this->vtxs;
}

const std::vector<Curve *> &
Mesh::curves() const
{
    return this->crvs;
}

const std::vector<Point *> &
Mesh::points() const
{
    return this->pts;
}

const std::vector<Line2 *> &
Mesh::lines() const
{
    return this->lns;
}

} // namespace krado::mesh
