#include "krado/mesh.h"
#include "krado/model.h"
#include "krado/scheme1d.h"
#include "krado/scheme2d.h"
#include "krado/scheme3d.h"

namespace krado {

Mesh::Mesh() : scheme_factory(SchemeFactory::instance()), gid_ctr(0) {}

Mesh::Mesh(const Model & model) : scheme_factory(SchemeFactory::instance()), gid_ctr(0)
{
    initialize(model);
}

const MeshVertex &
Mesh::vertex(int id) const
{
    try {
        return this->vtxs.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

MeshVertex &
Mesh::vertex(int id)
{
    try {
        return this->vtxs.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

const std::map<int, MeshVertex> &
Mesh::vertices() const
{
    return this->vtxs;
}

const MeshCurve &
Mesh::curve(int id) const
{
    try {
        return this->crvs.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

MeshCurve &
Mesh::curve(int id)
{
    try {
        return this->crvs.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

const std::map<int, MeshCurve> &
Mesh::curves() const
{
    return this->crvs;
}

const MeshSurface &
Mesh::surface(int id) const
{
    try {
        return this->surfs.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

MeshSurface &
Mesh::surface(int id)
{
    try {
        return this->surfs.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

const std::map<int, MeshSurface> &
Mesh::surfaces() const
{
    return this->surfs;
}

const MeshVolume &
Mesh::volume(int id) const
{
    try {
        return this->vols.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

MeshVolume &
Mesh::volume(int id)
{
    try {
        return this->vols.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
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
        std::vector<MeshCurve *> mesh_curves;
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
        std::vector<MeshSurface *> mesh_surfaces;
        for (auto & gsurface : volume_surfaces) {
            auto sid = model.surface_id(gsurface);
            auto * msurface = &surface(sid);
            mesh_surfaces.push_back(msurface);
        }

        MeshVolume mesh_vol(geom_volume, mesh_surfaces);
        this->vols.emplace(id, mesh_vol);
    }
}

void
Mesh::mesh_curve(int id)
{
    auto & curve = this->crvs.at(id);
    mesh_curve(curve);
}

void
Mesh::mesh_curve(MeshCurve & curve)
{
    if (!curve.is_meshed()) {
        auto mesh_pars = curve.meshing_parameters();
        auto scheme_name = mesh_pars.get<std::string>("scheme");
        auto scheme = get_scheme<Scheme1D>(scheme_name, *this, mesh_pars);
        scheme->mesh_curve(curve);
        curve.set_meshed();
    }
}

void
Mesh::mesh_surface(int id)
{
    auto & surface = this->surfs.at(id);
    mesh_surface(surface);
}

void
Mesh::mesh_surface(MeshSurface & surface)
{
    if (!surface.is_meshed()) {
        auto mesh_pars = surface.meshing_parameters();
        auto scheme_name = mesh_pars.get<std::string>("scheme");
        auto scheme = get_scheme<Scheme2D>(scheme_name, *this, mesh_pars);
        scheme->mesh_surface(surface);
        surface.set_meshed();
    }
}

void
Mesh::mesh_volume(int id)
{
    auto & volume = this->vols.at(id);
    mesh_volume(volume);
}

void
Mesh::mesh_volume(MeshVolume & volume)
{
    if (!volume.is_meshed()) {
        auto mesh_pars = volume.meshing_parameters();
        auto scheme_name = mesh_pars.get<std::string>("scheme");
        auto scheme = get_scheme<Scheme3D>(scheme_name, *this, mesh_pars);
        scheme->mesh_volume(volume);
        volume.set_meshed();
    }
}

void
Mesh::assign_gid(MeshVertex & vertex)
{
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

void
Mesh::assign_gid(MeshCurveVertex & vertex)
{
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

} // namespace krado
