#include "krado/mesh/mesh.h"
#include "krado/geo/model.h"

namespace krado::mesh {

Mesh::Mesh(const geo::Model & model)
{
    initialize(model);
}

const Vertex &
Mesh::vertex(int id) const
{
    return this->vtxs[0];
}

const std::vector<Vertex> &
Mesh::vertices() const
{
    return this->vtxs;
}

const Curve &
Mesh::curve(int id) const
{
    return this->crvs[0];
}

const std::vector<Curve> &
Mesh::curves() const
{
    return this->crvs;
}

void
Mesh::initialize(const geo::Model & model)
{
    for (auto & [id, gvtx] : model.vertices()) {
        Vertex mvtx(gvtx);
        this->vtxs.emplace_back(mvtx);
    }

    for (auto & [id, geom_curve] : model.curves()) {
        auto id1 = model.vertex_id(geom_curve.first_vertex());
        auto id2 = model.vertex_id(geom_curve.last_vertex());
        auto v1 = vertex(id1);
        auto v2 = vertex(id2);

        Curve mesh_crv(geom_curve, &v1, &v2);
        this->crvs.emplace_back(mesh_crv);
    }
}

} // namespace krado::mesh
