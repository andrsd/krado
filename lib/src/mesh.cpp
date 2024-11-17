// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh.h"
#include "krado/geom_model.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/scheme1d.h"
#include "krado/scheme2d.h"
#include "krado/scheme3d.h"
#include <array>

namespace krado {

Mesh::Mesh() : scheme_factory(SchemeFactory::instance()), gid_ctr(0) {}

Mesh::Mesh(const GeomModel & model) : scheme_factory(SchemeFactory::instance()), gid_ctr(0)
{
    initialize(model);
}

Mesh::Mesh(std::vector<Point> points, std::vector<MeshElement> elements) :
    pnts(points),
    elems(elements),
    scheme_factory(SchemeFactory::instance()),
    gid_ctr(0)
{
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
Mesh::initialize(const GeomModel & model)
{
    for (auto & [id, gvtx] : model.vertices()) {
        MeshVertex mvtx(gvtx);
        this->vtxs.emplace(id, mvtx);
    }

    for (auto & [id, geom_curve] : model.curves()) {
        auto id1 = model.vertex_id(geom_curve.first_vertex());
        auto id2 = model.vertex_id(geom_curve.last_vertex());
        auto & v1 = vertex(id1);
        auto & v2 = vertex(id2);

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
Mesh::mesh_vertex(int id)
{
    auto & vertex = this->vtxs.at(id);
    mesh_vertex(vertex);
}

void
Mesh::mesh_vertex(MeshVertex & vertex)
{
    if (!vertex.is_meshed()) {
        vertex.set_meshed();
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
        auto & geom_curve = curve.geom_curve();
        if ((geom_curve.length() == 0.) || (geom_curve.is_degenerated()))
            return;

        auto & scheme = get_scheme<Scheme1D>(curve);

        auto bnd_vtxs = curve.bounding_vertices();
        for (auto & v : bnd_vtxs)
            mesh_vertex(*v);

        scheme.mesh_curve(curve);
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
        auto & scheme = get_scheme<Scheme2D>(surface);

        auto curves = surface.curves();
        for (auto & crv : curves)
            scheme.select_curve_scheme(*crv);
        for (auto & crv : curves)
            mesh_curve(*crv);

        scheme.mesh_surface(surface);
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
        auto & scheme = get_scheme<Scheme3D>(volume);
        scheme.mesh_volume(volume);
        volume.set_meshed();
    }
}

void
Mesh::assign_gid(MeshVertex & vertex)
{
    this->pnts.emplace_back(vertex.point());
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

void
Mesh::assign_gid(MeshCurveVertex & vertex)
{
    this->pnts.emplace_back(vertex.point());
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

void
Mesh::assign_gid(MeshSurfaceVertex & vertex)
{
    this->pnts.emplace_back(vertex.point());
    vertex.set_global_id(this->gid_ctr);
    this->gid_ctr++;
}

const std::vector<Point> &
Mesh::points() const
{
    return this->pnts;
}

const std::vector<MeshElement> &
Mesh::elements() const
{
    return this->elems;
}

void
Mesh::add_mesh_point(Point & mpnt)
{
    this->pnts.emplace_back(mpnt);
}

void
Mesh::number_points()
{
    this->exp_bbox.reset();
    for (auto & [id, v] : this->vtxs)
        if (v.is_meshed()) {
            assign_gid(v);
            this->exp_bbox += v.point();
        }
    for (auto & [id, curve] : this->crvs)
        if (curve.is_meshed())
            for (auto & v : curve.curve_vertices()) {
                assign_gid(*v);
                this->exp_bbox += v->point();
            }
    for (auto & [id, surface] : this->surfs)
        if (surface.is_meshed())
            for (auto & v : surface.surface_vertices()) {
                assign_gid(*v);
                this->exp_bbox += v->point();
            }
}

void
Mesh::build_elements()
{
    auto dims = this->exp_bbox.size();
    if ((dims[0] > 0) && (dims[1] < 1e-15) && (dims[2] < 1e-15))
        build_1d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] < 1e-15))
        build_2d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] > 0))
        throw Exception("3D element construction is not implemented yet");
    else
        throw Exception("Element construction for your setup is not implemented yet");
}

void
Mesh::build_1d_elements()
{
    for (auto & [id, curve] : this->crvs) {
        if (curve.is_meshed()) {
            auto verts = curve.all_vertices();
            std::array<int, 2> line;
            for (auto & local_elem : curve.segments()) {
                for (int i = 0; i < 2; i++) {
                    auto lid = local_elem.ids()[i];
                    auto gid = verts[lid]->global_id();
                    line[i] = gid;
                }
                this->elems.emplace_back(MeshElement::Line2(line));
            }
        }
    }
}

void
Mesh::build_2d_elements()
{
    for (auto & [id, surface] : this->surfs) {
        if (surface.is_meshed()) {
            auto verts = surface.all_vertices();
            std::array<int, 3> tri;
            for (auto & local_elem : surface.triangles()) {
                for (int i = 0; i < 3; i++) {
                    auto lid = local_elem.ids()[i];
                    auto gid = verts[lid]->global_id();
                    tri[i] = gid;
                }
                this->elems.emplace_back(MeshElement::Tri3(tri));
            }
        }
    }
}

BoundingBox3D
Mesh::bounding_box() const
{
    return this->exp_bbox;
}

Mesh
Mesh::scaled(double factor) const
{
    auto tr = Trsf::scale(factor);
    return transformed(tr);
}

Mesh
Mesh::scaled(double factor_x, double factor_y, double factor_z) const
{
    auto tr = Trsf::scale(factor_x, factor_y, factor_z);
    return transformed(tr);
}

Mesh
Mesh::translated(double tx, double ty, double tz) const
{
    auto tr = Trsf::translate(tx, ty, tz);
    return transformed(tr);
}

Mesh
Mesh::transformed(const Trsf & tr) const
{
    auto pts = points();
    for (auto & p : pts)
        p = tr * p;
    auto elems = elements();
    return Mesh(pts, elems);
}

void
Mesh::add(const Mesh & other)
{
    auto n_pt_ofst = this->pnts.size();
    this->pnts.insert(this->pnts.end(), other.pnts.begin(), other.pnts.end());
    for (auto & elem : other.elements()) {
        auto ids = elem.ids();
        for (auto & id : ids)
            id += n_pt_ofst;
        auto new_elem = MeshElement(elem.type(), ids);
        this->elems.emplace_back(new_elem);
    }
}

} // namespace krado
