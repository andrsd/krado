#include "krado/mesh.h"
#include "krado/model.h"

namespace krado {

Mesh::Mesh(const Model & model)
{
    initialize(model);
}

const MeshVertex &
Mesh::vertex(int id) const
{
    return this->vtxs[0];
}

const std::vector<MeshVertex> &
Mesh::vertices() const
{
    return this->vtxs;
}

const MeshCurve &
Mesh::curve(int id) const
{
    return this->crvs[0];
}

const std::vector<MeshCurve> &
Mesh::curves() const
{
    return this->crvs;
}

const MeshSurface &
Mesh::surface(int id) const
{
    return this->surfs[0];
}

const std::vector<MeshSurface> &
Mesh::surfaces() const
{
    return this->surfs;
}

void
Mesh::initialize(const Model & model)
{
    for (auto & gvtx : model.vertices()) {
        MeshVertex mvtx(gvtx);
        this->vtxs.emplace_back(mvtx);
    }

    for (auto & geom_curve : model.curves()) {
        auto id1 = model.vertex_id(geom_curve.first_vertex());
        auto id2 = model.vertex_id(geom_curve.last_vertex());
        auto v1 = vertex(id1);
        auto v2 = vertex(id2);

        MeshCurve mesh_crv(geom_curve, &v1, &v2);
        this->crvs.emplace_back(mesh_crv);
    }

    for (auto & geom_surface : model.surfaces()) {
        MeshSurface mesh_surf(geom_surface);
        this->surfs.emplace_back(mesh_surf);
    }
}

} // namespace krado
