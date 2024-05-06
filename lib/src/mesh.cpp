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
    return this->vtxs.at(id);
}

const std::map<int, MeshVertex> &
Mesh::vertices() const
{
    return this->vtxs;
}

const MeshCurve &
Mesh::curve(int id) const
{
    return this->crvs.at(id);
}

const std::map<int, MeshCurve> &
Mesh::curves() const
{
    return this->crvs;
}

const MeshSurface &
Mesh::surface(int id) const
{
    return this->surfs.at(id);
}

const std::map<int, MeshSurface> &
Mesh::surfaces() const
{
    return this->surfs;
}

const MeshVolume &
Mesh::volume(int id) const
{
    return this->vols.at(id);
}

const std::map<int, MeshVolume> &
Mesh::volumes() const
{
    return this->vols;
}

void
Mesh::initialize(const Model & model)
{
    for (auto & [id, gvtx] : model.vertices()) {
        MeshVertex mvtx(gvtx);
        this->vtxs.emplace(id, mvtx);
    }

    for (auto & [id, geom_curve] : model.curves()) {
        auto id1 = model.vertex_id(geom_curve.first_vertex());
        auto id2 = model.vertex_id(geom_curve.last_vertex());
        auto v1 = vertex(id1);
        auto v2 = vertex(id2);

        MeshCurve mesh_crv(geom_curve, &v1, &v2);
        this->crvs.emplace(id, mesh_crv);
    }

    for (auto & [id, geom_surface] : model.surfaces()) {
        auto surface_curves = geom_surface.curves();
        std::vector<const MeshCurve *> mesh_curves;
        for (auto & gcurve : surface_curves) {
            int cid = model.curve_id(gcurve);
            auto * mcurve = &curve(cid);
            mesh_curves.push_back(mcurve);
        }

        MeshSurface mesh_surf(geom_surface, mesh_curves);
        this->surfs.emplace(id, mesh_surf);
    }

    for (auto & [id, geom_volume] : model.volumes()) {
        auto volume_surfaces = geom_volume.surfaces();
        std::vector<const MeshSurface *> mesh_surfaces;
        for (auto & gsurface : volume_surfaces) {
            auto sid = model.surface_id(gsurface);
            auto * msurface = &surface(sid);
            mesh_surfaces.push_back(msurface);
        }

        MeshVolume mesh_vol(geom_volume, mesh_surfaces);
        this->vols.emplace(id, mesh_vol);
    }
}

} // namespace krado
