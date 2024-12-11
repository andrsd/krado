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

GeomModel::GeomModel(const GeomShape & root_shape) : root_shape(root_shape)
{
    bind_shape(root_shape);
    initialize();
}

const GeomVertex &
GeomModel::geom_vertex(int id) const
{
    try {
        return this->vtxs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

GeomVertex &
GeomModel::geom_vertex(int id)
{
    try {
        return this->vtxs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

int
GeomModel::vertex_id(const GeomVertex & vertex) const
{
    try {
        return this->vtx_id.Find(vertex);
    }
    catch (...) {
        throw Exception("No ID for vertex");
    }
}

const GeomCurve &
GeomModel::geom_curve(int id) const
{
    try {
        return this->crvs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

GeomCurve &
GeomModel::geom_curve(int id)
{
    try {
        return this->crvs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

int
GeomModel::curve_id(const GeomCurve & curve) const
{
    try {
        return this->crv_id.Find(curve);
    }
    catch (...) {
        throw Exception("No ID for curve");
    }
}

const GeomSurface &
GeomModel::geom_surface(int id) const
{
    try {
        return this->srfs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

GeomSurface &
GeomModel::geom_surface(int id)
{
    try {
        return this->srfs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

int
GeomModel::surface_id(const GeomSurface & surface) const
{
    try {
        return this->srf_id.Find(surface);
    }
    catch (...) {
        throw Exception("No ID for surface");
    }
}

const GeomVolume &
GeomModel::geom_volume(int id) const
{
    try {
        return this->vols.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

GeomVolume &
GeomModel::geom_volume(int id)
{
    try {
        return this->vols.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

int
GeomModel::volume_id(const GeomVolume & volume) const
{
    try {
        return this->vol_id.Find(volume);
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
        if (!this->vol_id.IsBound(solid)) {
            auto id = this->vols.size() + 1;
            this->vols.emplace(id, GeomVolume(solid));
            this->vol_id.Bind(solid, id);
        }
    }
}

void
GeomModel::bind_faces(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());
        if (!this->srf_id.IsBound(face)) {
            auto id = this->srfs.size() + 1;
            this->srfs.emplace(id, GeomSurface(face));
            this->srf_id.Bind(face, id);
        }
    }
}

void
GeomModel::bind_edges(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_EDGE); exp0.More(); exp0.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp0.Current());
        if (!this->crv_id.IsBound(edge)) {
            auto id = this->crvs.size() + 1;
            this->crvs.emplace(id, GeomCurve(edge));
            this->crv_id.Bind(edge, id);
        }
    }
}

void
GeomModel::bind_vertices(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_VERTEX); exp0.More(); exp0.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(exp0.Current());
        if (!this->vtx_id.IsBound(vertex)) {
            auto id = this->vtxs.size() + 1;
            this->vtxs.emplace(id, GeomVertex(vertex));
            this->vtx_id.Bind(vertex, id);
        }
    }
}

void
GeomModel::initialize()
{
    for (auto & [id, gvtx] : this->vtxs) {
        MeshVertex mvtx(gvtx);
        this->mvtxs.emplace(id, mvtx);
    }

    for (auto & [id, geom_curve] : this->crvs) {
        auto id1 = vertex_id(geom_curve.first_vertex());
        auto id2 = vertex_id(geom_curve.last_vertex());
        auto & v1 = vertex(id1);
        auto & v2 = vertex(id2);

        MeshCurve mesh_crv(geom_curve, &v1, &v2);
        this->mcrvs.emplace(id, mesh_crv);
    }

    for (auto & [id, geom_surface] : this->srfs) {
        auto surface_curves = geom_surface.curves();
        std::vector<MeshCurve *> mesh_curves;
        for (auto & gcurve : surface_curves) {
            auto cid = curve_id(gcurve);
            auto * mcurve = &curve(cid);
            mesh_curves.push_back(mcurve);
        }

        MeshSurface mesh_surf(geom_surface, mesh_curves);
        this->msurfs.emplace(id, mesh_surf);
    }

    for (auto & [id, geom_volume] : this->vols) {
        auto volume_surfaces = geom_volume.surfaces();
        std::vector<MeshSurface *> mesh_surfaces;
        for (auto & gsurface : volume_surfaces) {
            auto sid = surface_id(gsurface);
            auto * msurface = &surface(sid);
            mesh_surfaces.push_back(msurface);
        }

        MeshVolume mesh_vol(geom_volume, mesh_surfaces);
        this->mvols.emplace(id, mesh_vol);
    }
}

const MeshVertex &
GeomModel::vertex(int id) const
{
    try {
        return this->mvtxs.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

MeshVertex &
GeomModel::vertex(int id)
{
    try {
        return this->mvtxs.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

const std::map<int, MeshVertex> &
GeomModel::vertices() const
{
    return this->mvtxs;
}

const MeshCurve &
GeomModel::curve(int id) const
{
    try {
        return this->mcrvs.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

MeshCurve &
GeomModel::curve(int id)
{
    try {
        return this->mcrvs.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

const std::map<int, MeshCurve> &
GeomModel::curves() const
{
    return this->mcrvs;
}

const MeshSurface &
GeomModel::surface(int id) const
{
    try {
        return this->msurfs.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

MeshSurface &
GeomModel::surface(int id)
{
    try {
        return this->msurfs.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

const std::map<int, MeshSurface> &
GeomModel::surfaces() const
{
    return this->msurfs;
}

const MeshVolume &
GeomModel::volume(int id) const
{
    try {
        return this->mvols.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

MeshVolume &
GeomModel::volume(int id)
{
    try {
        return this->mvols.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

const std::map<int, MeshVolume> &
GeomModel::volumes() const
{
    return this->mvols;
}

void
GeomModel::mesh_vertex(int id)
{
    auto & vertex = this->mvtxs.at(id);
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
    auto & curve = this->mcrvs.at(id);
    mesh_curve(curve);
}

void
GeomModel::mesh_curve(MeshCurve & curve)
{
    if (!curve.is_meshed()) {
        auto geom_curve = curve.geom_curve();
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
GeomModel::mesh_surface(int id)
{
    auto & surface = this->msurfs.at(id);
    mesh_surface(surface);
}

void
GeomModel::mesh_surface(MeshSurface & surface)
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
GeomModel::mesh_volume(int id)
{
    auto & volume = this->mvols.at(id);
    mesh_volume(volume);
}

void
GeomModel::mesh_volume(MeshVolume & volume)
{
    if (!volume.is_meshed()) {
        auto & scheme = get_scheme<Scheme3D>(volume);
        scheme.mesh_volume(volume);
        volume.set_meshed();
    }
}

std::vector<Point>
GeomModel::build_points()
{
    std::vector<Point> pnts;
    gidx_t gid = 0;

    for (auto & [id, v] : this->mvtxs) {
        v.set_global_id(gid);
        pnts.emplace_back(v.point());
        gid++;
    }
    for (auto & [id, curve] : this->mcrvs)
        for (auto & v : curve.curve_vertices()) {
            v->set_global_id(gid);
            pnts.emplace_back(v->point());
            gid++;
        }
    for (auto & [id, surface] : this->msurfs)
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
    for (auto & [id, v] : this->mvtxs)
        bbox += v.point();
    for (auto & [id, curve] : this->mcrvs)
        for (auto & v : curve.curve_vertices())
            bbox += v->point();
    for (auto & [id, surface] : this->msurfs)
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
GeomModel::build_1d_elements()
{
    std::vector<Element> elems;
    for (auto & [id, curve] : this->mcrvs) {
        auto verts = curve.all_vertices();
        std::array<gidx_t, 2> line;
        for (auto & local_elem : curve.segments()) {
            for (int i = 0; i < 2; i++) {
                auto lid = local_elem.ids()[i];
                auto gid = verts[lid]->global_id();
                line[i] = gid;
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
    for (auto & [id, surface] : this->msurfs) {
        auto verts = surface.all_vertices();
        std::array<gidx_t, 3> tri;
        for (auto & local_elem : surface.triangles()) {
            for (int i = 0; i < 3; i++) {
                auto lid = local_elem.ids()[i];
                auto gid = verts[lid]->global_id();
                tri[i] = gid;
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

} // namespace krado
