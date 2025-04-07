// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_model.h"
#include "krado/exception.h"
#include "krado/scheme1d.h"
#include "krado/scheme2d.h"
#include "krado/scheme3d.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"
#include "krado/types.h"

namespace krado {

GeomModel::GeomModel(const GeomShape & root_shape) : root_shape_(root_shape)
{
    bind_shape(root_shape);
    initialize();
}

const GeomVertex &
GeomModel::geom_vertex(int id) const
{
    try {
        return this->vtxs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

GeomVertex &
GeomModel::geom_vertex(int id)
{
    try {
        return this->vtxs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

int
GeomModel::vertex_id(const GeomVertex & vertex) const
{
    try {
        return this->vtx_id_.Find(vertex);
    }
    catch (...) {
        throw Exception("No ID for vertex");
    }
}

const GeomCurve &
GeomModel::geom_curve(int id) const
{
    try {
        return this->crvs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

GeomCurve &
GeomModel::geom_curve(int id)
{
    try {
        return this->crvs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

int
GeomModel::curve_id(const GeomCurve & curve) const
{
    try {
        return this->crv_id_.Find(curve);
    }
    catch (...) {
        throw Exception("No ID for curve");
    }
}

const GeomSurface &
GeomModel::geom_surface(int id) const
{
    try {
        return this->srfs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

GeomSurface &
GeomModel::geom_surface(int id)
{
    try {
        return this->srfs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

int
GeomModel::surface_id(const GeomSurface & surface) const
{
    try {
        return this->srf_id_.Find(surface);
    }
    catch (...) {
        throw Exception("No ID for surface");
    }
}

const GeomVolume &
GeomModel::geom_volume(int id) const
{
    try {
        return this->vols_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

GeomVolume &
GeomModel::geom_volume(int id)
{
    try {
        return this->vols_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

int
GeomModel::volume_id(const GeomVolume & volume) const
{
    try {
        return this->vol_id_.Find(volume);
    }
    catch (...) {
        throw Exception("No ID for volume");
    }
}

void
GeomModel::bind_shape(const GeomShape & shape)
{
    bind_vertices(shape);
    bind_edges(shape);
    bind_faces(shape);
    bind_solids(shape);
}

void
GeomModel::bind_solids(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_SOLID); exp0.More(); exp0.Next()) {
        TopoDS_Solid solid = TopoDS::Solid(exp0.Current());
        if (!this->vol_id_.IsBound(solid)) {
            auto id = this->vols_.size() + 1;
            GeomVolume gvol(solid);
            gvol.set_id(id);
            this->vols_.emplace(id, gvol);
            this->vol_id_.Bind(solid, id);
        }
    }
}

void
GeomModel::bind_faces(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());
        if (!this->srf_id_.IsBound(face)) {
            auto id = this->srfs_.size() + 1;
            GeomSurface gsurf(face);
            gsurf.set_id(id);
            this->srfs_.emplace(id, gsurf);
            this->srf_id_.Bind(face, id);
        }
    }
}

void
GeomModel::bind_edges(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_EDGE); exp0.More(); exp0.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp0.Current());
        if (!this->crv_id_.IsBound(edge)) {
            auto id = this->crvs_.size() + 1;
            GeomCurve gedge(edge);
            gedge.set_id(id);
            this->crvs_.emplace(id, gedge);
            this->crv_id_.Bind(edge, id);
        }
    }
}

void
GeomModel::bind_vertices(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_VERTEX); exp0.More(); exp0.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(exp0.Current());
        if (!this->vtx_id_.IsBound(vertex)) {
            auto id = this->vtxs_.size() + 1;
            GeomVertex gvtx(vertex);
            gvtx.set_id(id);
            this->vtxs_.emplace(id, gvtx);
            this->vtx_id_.Bind(vertex, id);
        }
    }
}

void
GeomModel::initialize()
{
    for (auto & [id, gvtx] : this->vtxs_) {
        MeshVertex mvtx(gvtx);
        this->mvtxs_.emplace(id, mvtx);
    }

    for (auto & [id, geom_curve] : this->crvs_) {
        auto id1 = vertex_id(geom_curve.first_vertex());
        auto id2 = vertex_id(geom_curve.last_vertex());
        auto & v1 = vertex(id1);
        auto & v2 = vertex(id2);

        MeshCurve mesh_crv(geom_curve, &v1, &v2);
        this->mcrvs_.emplace(id, mesh_crv);
    }

    for (auto & [id, geom_surface] : this->srfs_) {
        auto surface_curves = geom_surface.curves();
        std::vector<MeshCurve *> mesh_curves;
        for (auto & gcurve : surface_curves) {
            auto cid = curve_id(gcurve);
            auto * mcurve = &curve(cid);
            mesh_curves.push_back(mcurve);
        }

        MeshSurface mesh_surf(geom_surface, mesh_curves);
        this->msurfs_.emplace(id, mesh_surf);
    }

    for (auto & [id, geom_volume] : this->vols_) {
        auto volume_surfaces = geom_volume.surfaces();
        std::vector<MeshSurface *> mesh_surfaces;
        for (auto & gsurface : volume_surfaces) {
            auto sid = surface_id(gsurface);
            auto * msurface = &surface(sid);
            mesh_surfaces.push_back(msurface);
        }

        MeshVolume mesh_vol(geom_volume, mesh_surfaces);
        this->mvols_.emplace(id, mesh_vol);
    }
}

const MeshVertex &
GeomModel::vertex(int id) const
{
    try {
        return this->mvtxs_.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

MeshVertex &
GeomModel::vertex(int id)
{
    try {
        return this->mvtxs_.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

const std::map<int, MeshVertex> &
GeomModel::vertices() const
{
    return this->mvtxs_;
}

const MeshCurve &
GeomModel::curve(int id) const
{
    try {
        return this->mcrvs_.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

MeshCurve &
GeomModel::curve(int id)
{
    try {
        return this->mcrvs_.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

const std::map<int, MeshCurve> &
GeomModel::curves() const
{
    return this->mcrvs_;
}

const MeshSurface &
GeomModel::surface(int id) const
{
    try {
        return this->msurfs_.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

MeshSurface &
GeomModel::surface(int id)
{
    try {
        return this->msurfs_.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

const std::map<int, MeshSurface> &
GeomModel::surfaces() const
{
    return this->msurfs_;
}

const MeshVolume &
GeomModel::volume(int id) const
{
    try {
        return this->mvols_.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

MeshVolume &
GeomModel::volume(int id)
{
    try {
        return this->mvols_.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

const std::map<int, MeshVolume> &
GeomModel::volumes() const
{
    return this->mvols_;
}

void
GeomModel::mesh_vertex(int id)
{
    auto & vertex = this->mvtxs_.at(id);
    mesh_vertex(vertex);
}

void
GeomModel::mesh_vertex(MeshVertex & gvertex)
{
    if (!gvertex.is_meshed()) {
        gvertex.set_meshed();
    }
}

void
GeomModel::mesh_curve(int id)
{
    auto & curve = this->mcrvs_.at(id);
    mesh_curve(curve);
}

void
GeomModel::mesh_curve(MeshCurve & curve)
{
    if (!curve.is_meshed()) {
        try {
            auto & geom_curve = curve.geom_curve();
            if ((geom_curve.length() == 0.) || (geom_curve.is_degenerated()))
                return;

            auto & scheme1d = get_scheme<Scheme1D>(curve);

            auto & bnd_vtxs = curve.bounding_vertices();
            for (auto & v : bnd_vtxs)
                mesh_vertex(*v);

            scheme1d.mesh_curve(curve);
            curve.set_meshed();
        }
        catch (const std::bad_cast & e) {
            throw Exception("Scheme '{}' is not a 1D scheme", curve.scheme().name());
        }
    }
}

void
GeomModel::mesh_surface(int id)
{
    auto & surface = this->msurfs_.at(id);
    mesh_surface(surface);
}

void
GeomModel::mesh_surface(MeshSurface & surface)
{
    if (!surface.is_meshed()) {
        try {
            auto & scheme2d = get_scheme<Scheme2D>(surface);

            auto & curves = surface.curves();
            for (auto & crv : curves)
                scheme2d.select_curve_scheme(*crv);
            for (auto & crv : curves)
                mesh_curve(*crv);

            scheme2d.mesh_surface(surface);
            surface.set_meshed();
        }
        catch (const std::bad_cast & e) {
            throw Exception("Scheme '{}' is not a 2D scheme", surface.scheme().name());
        }
    }
}

void
GeomModel::mesh_volume(int id)
{
    auto & volume = this->mvols_.at(id);
    mesh_volume(volume);
}

void
GeomModel::mesh_volume(MeshVolume & volume)
{
    if (!volume.is_meshed()) {
        try {
            auto & scheme3d = get_scheme<Scheme3D>(volume);

            auto & surfaces = volume.surfaces();
            for (auto & srf : surfaces)
                scheme3d.select_surface_scheme(*srf);
            for (auto & srf : surfaces)
                mesh_surface(*srf);

            scheme3d.mesh_volume(volume);
            volume.set_meshed();
        }
        catch (const std::bad_cast & e) {
            throw Exception("Scheme '{}' is not a 3D scheme", volume.scheme().name());
        }
    }
}

std::vector<Point>
GeomModel::build_points()
{
    std::vector<Point> pnts;
    gidx_t gid = 0;

    for (auto & [id, v] : this->mvtxs_) {
        v.set_global_id(gid);
        pnts.emplace_back(v.point());
        gid++;
    }
    for (auto & [id, curve] : this->mcrvs_)
        for (auto & v : curve.curve_vertices()) {
            v->set_global_id(gid);
            pnts.emplace_back(v->point());
            gid++;
        }
    for (auto & [id, surface] : this->msurfs_)
        for (auto & v : surface.surface_vertices()) {
            v->set_global_id(gid);
            pnts.emplace_back(v->point());
            gid++;
        }
    return pnts;
}

BoundingBox3D
GeomModel::compute_mesh_bounding_box()
{
    BoundingBox3D bbox;
    for (auto & [id, v] : this->mvtxs_)
        bbox += v.point();
    for (auto & [id, curve] : this->mcrvs_)
        for (auto & v : curve.curve_vertices())
            bbox += v->point();
    for (auto & [id, surface] : this->msurfs_)
        for (auto & v : surface.surface_vertices())
            bbox += v->point();
    return bbox;
}

std::vector<Element>
GeomModel::build_elements()
{
    auto bbox = compute_mesh_bounding_box();
    auto dims = bbox.size();

    if ((dims[0] > 0) && (dims[1] < 1e-15) && (dims[2] < 1e-15))
        return build_1d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] < 1e-15))
        return build_2d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] > 0))
        throw Exception("3D element construction is not implemented yet");
    else
        throw Exception("Element construction for your setup is not implemented yet");
}

std::vector<Element>
GeomModel::build_surface_elements()
{
    auto bbox = compute_mesh_bounding_box();
    auto dims = bbox.size();

    if ((dims[0] > 0) && (dims[1] < 1e-15) && (dims[2] < 1e-15))
        throw Exception("Surface mesh in 1D is not implemented yet");
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] < 1e-15))
        return build_1d_elements();
    else if ((dims[0] > 0) && (dims[1] > 0) && (dims[2] > 0))
        return build_2d_elements();
    else
        throw Exception("Element construction for your setup is not implemented yet");
}

std::vector<Element>
GeomModel::build_1d_elements()
{
    std::vector<Element> elems;
    for (auto & [id, curve] : this->mcrvs_) {
        auto & verts = curve.all_vertices();
        std::array<gidx_t, Line2::N_VERTICES> line;
        for (auto & local_elem : curve.segments()) {
            for (int i = 0; i < Line2::N_VERTICES; ++i) {
                auto * vtx = local_elem.vertex(i);
                line[i] = vtx->global_id();
            }
            elems.emplace_back(Element::Line2(line));
        }
    }
    return elems;
}

std::vector<Element>
GeomModel::build_2d_elements()
{
    std::vector<Element> elems;
    for (auto & [id, surface] : this->msurfs_) {
        auto & verts = surface.all_vertices();
        auto & tris = surface.triangles();
        std::array<gidx_t, Tri3::N_VERTICES> tri;
        for (auto & local_elem : tris) {
            for (int i = 0; i < Tri3::N_VERTICES; ++i) {
                auto * vtx = local_elem.vertex(i);
                tri[i] = vtx->global_id();
            }
            elems.emplace_back(Element::Tri3(tri));
        }
    }
    return elems;
}

Mesh
GeomModel::build_mesh()
{
    auto points = build_points();
    auto elements = build_elements();
    Mesh mesh(points, elements);
    return mesh;
}

Mesh
GeomModel::build_surface_mesh()
{
    auto points = build_points();
    auto elements = build_surface_elements();
    Mesh mesh(points, elements);
    return mesh;
}

} // namespace krado
