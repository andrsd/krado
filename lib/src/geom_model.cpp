// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_model.h"
#include "krado/bounding_box_3d.h"
#include "krado/exception.h"
#include "krado/scheme.h"
#include "krado/scheme1d.h"
#include "krado/scheme2d.h"
#include "krado/scheme3d.h"
#include "krado/mesh.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_volume.h"
#include "krado/log.h"
#include "krado/timer.h"
#include "krado/types.h"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Shell.hxx"
#include "TopoDS_Solid.hxx"

namespace krado {

BoundingBox3D
compute_bounding_box(const GeomModel & model)
{
    Log::debug("Computing mesh bounding box");

    BoundingBox3D bbox;
    for (auto & [id, v] : model.vertices())
        bbox += v->point();
    for (auto & [id, curve] : model.curves())
        for (auto & v : curve->curve_vertices())
            bbox += v->point();
    for (auto & [id, surface] : model.surfaces())
        for (auto & v : surface->surface_vertices())
            bbox += v->point();
    return bbox;
}

//

GeomModel::GeomModel(const GeomShape & root_shape) : root_shape_(root_shape)
{
    Log::debug("Creating geometrical model");

    bind_shape(root_shape);
    initialize();
}

GeomModel::~GeomModel() = default;

const GeomVertex &
GeomModel::geom_vertex(ShapeID id) const
{
    try {
        return this->vtxs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

GeomVertex &
GeomModel::geom_vertex(ShapeID id)
{
    try {
        return this->vtxs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

ShapeID
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
GeomModel::geom_curve(ShapeID id) const
{
    try {
        return this->crvs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

GeomCurve &
GeomModel::geom_curve(ShapeID id)
{
    try {
        return this->crvs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

ShapeID
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
GeomModel::geom_surface(ShapeID id) const
{
    try {
        return this->srfs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

GeomSurface &
GeomModel::geom_surface(ShapeID id)
{
    try {
        return this->srfs_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

ShapeID
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
GeomModel::geom_volume(ShapeID id) const
{
    try {
        return this->vols_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

GeomVolume &
GeomModel::geom_volume(ShapeID id)
{
    try {
        return this->vols_.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

ShapeID
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
            auto id = get_shape_id(solid);
            this->vol_id_.Bind(solid, id);

            GeomVolume gvol(solid);
            gvol.id_ = id;
            gvol.set_material(shape.material(), shape.material_description(), shape.density());
            this->vols_.emplace(id, gvol);
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
            auto id = get_shape_id(face);
            this->srf_id_.Bind(face, id);

            GeomSurface gsurf(face);
            gsurf.id_ = id;
            gsurf.set_material(shape.material(), shape.material_description(), shape.density());
            this->srfs_.emplace(id, gsurf);
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
            auto id = get_shape_id(edge);
            this->crv_id_.Bind(edge, id);

            GeomCurve gedge(edge);
            gedge.id_ = id;
            gedge.set_material(shape.material(), shape.material_description(), shape.density());
            this->crvs_.emplace(id, gedge);
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
            auto id = get_shape_id(vertex);
            this->vtx_id_.Bind(vertex, id);

            GeomVertex gvtx(vertex);
            gvtx.id_ = id;
            gvtx.set_material(shape.material(), shape.material_description(), shape.density());
            this->vtxs_.emplace(id, gvtx);
        }
    }
}

void
GeomModel::initialize()
{
    for (auto & [id, gvtx] : this->vtxs_) {
        auto mvtx = Ptr<MeshVertex>::alloc(id, gvtx);
        this->mvtxs_.emplace(id, mvtx);
    }

    for (auto & [id, geom_curve] : this->crvs_) {
        auto id1 = vertex_id(geom_curve.first_vertex());
        auto id2 = vertex_id(geom_curve.last_vertex());
        auto v1 = vertex(id1);
        auto v2 = vertex(id2);

        auto mesh_crv = Ptr<MeshCurve>::alloc(id, geom_curve, v1, v2);
        this->mcrvs_.emplace(id, mesh_crv);
    }

    for (auto & [id, geom_surface] : this->srfs_) {
        auto surface_curves = geom_surface.curves();
        std::vector<Ptr<MeshCurve>> mesh_curves;
        for (auto & gcurve : surface_curves) {
            auto cid = curve_id(gcurve);
            auto mcurve = curve(cid);
            mesh_curves.push_back(mcurve);
        }

        auto mesh_surf = Ptr<MeshSurface>::alloc(id, geom_surface, std::move(mesh_curves));
        this->msurfs_.emplace(id, mesh_surf);
    }

    for (auto & [id, geom_volume] : this->vols_) {
        auto volume_surfaces = geom_volume.surfaces();
        std::vector<Ptr<MeshSurface>> mesh_surfaces;
        for (auto & gsurface : volume_surfaces) {
            auto sid = surface_id(gsurface);
            auto msurface = surface(sid);
            mesh_surfaces.push_back(msurface);
        }

        auto mesh_vol = Ptr<MeshVolume>::alloc(id, geom_volume, std::move(mesh_surfaces));
        this->mvols_.emplace(id, mesh_vol);
    }
}

Ptr<MeshVertex>
GeomModel::vertex(ShapeID id)
{
    try {
        return this->mvtxs_.at(id);
    }
    catch (...) {
        throw Exception("No vertex with ID = {}", id);
    }
}

const std::map<ShapeID, Ptr<MeshVertex>> &
GeomModel::vertices() const
{
    return this->mvtxs_;
}

Ptr<MeshCurve>
GeomModel::curve(ShapeID id)
{
    try {
        return this->mcrvs_.at(id);
    }
    catch (...) {
        throw Exception("No curve with ID = {}", id);
    }
}

const std::map<ShapeID, Ptr<MeshCurve>> &
GeomModel::curves() const
{
    return this->mcrvs_;
}

Ptr<MeshSurface>
GeomModel::surface(ShapeID id)
{
    try {
        return this->msurfs_.at(id);
    }
    catch (...) {
        throw Exception("No surface with ID = {}", id);
    }
}

const std::map<ShapeID, Ptr<MeshSurface>> &
GeomModel::surfaces() const
{
    return this->msurfs_;
}

Ptr<MeshVolume>
GeomModel::volume(ShapeID id)
{
    try {
        return this->mvols_.at(id);
    }
    catch (...) {
        throw Exception("No volume with ID = {}", id);
    }
}

const std::map<ShapeID, Ptr<MeshVolume>> &
GeomModel::volumes() const
{
    return this->mvols_;
}

void
GeomModel::mesh_vertex(ShapeID id)
{
    auto & vertex = this->mvtxs_.at(id);
    mesh_vertex(vertex);
}

void
GeomModel::mesh_vertex(Ptr<MeshVertex> gvertex)
{
    Log::debug("Meshing vertex: id={}", gvertex->id());

    if (!gvertex->is_meshed()) {
        gvertex->set_meshed();
    }
}

void
GeomModel::mesh_curve(ShapeID id)
{
    auto curve = this->mcrvs_.at(id);
    mesh_curve(curve);
}

void
GeomModel::mesh_curve(Ptr<MeshCurve> curve)
{
    if (curve->is_meshed())
        return;

    auto & scheme = curve->scheme();

    auto & geom_curve = curve->geom_curve();
    if ((geom_curve.length() == 0.) || (geom_curve.is_degenerated()))
        return;

    auto bnd_vtxs = curve->bounding_vertices();
    for (auto & v : bnd_vtxs)
        mesh_vertex(v);

    {
        auto & s = dynamic_cast<Scheme &>(scheme);
        auto pars_str = s.params_to_str();
        Log::info("Meshing curve {} ({}): scheme='{}'{}",
                  curve->id(),
                  curve->geom_curve().type(),
                  s.name(),
                  pars_str.empty() ? "" : fmt::format(", {}", pars_str));
        LoggingTimer timer;
        scheme.mesh_curve(curve);
    }
    Log::info("- created {} segment(s)", utils::human_number(curve->segments().size()));

    curve->set_meshed();
}

void
GeomModel::mesh_surface(ShapeID id)
{
    auto surface = this->msurfs_.at(id);
    mesh_surface(surface);
}

void
GeomModel::mesh_surface(Ptr<MeshSurface> surface)
{
    if (surface->is_meshed())
        return;

    auto & scheme = surface->scheme();

    auto curves = surface->curves();
    for (auto & crv : curves)
        scheme.select_curve_scheme(crv);
    for (auto & crv : curves)
        mesh_curve(crv);

    {
        auto & s = dynamic_cast<Scheme &>(scheme);
        auto pars_str = s.params_to_str();
        Log::info("Meshing surface {}: scheme='{}'{}",
                  surface->id(),
                  s.name(),
                  pars_str.empty() ? "" : fmt::format(", {}", pars_str));
        LoggingTimer timer;
        scheme.mesh_surface(surface);
    }
    if (surface->triangles().size() > 0)
        Log::info("- created {} triangles(s)", utils::human_number(surface->triangles().size()));
    if (surface->quadrangles().size() > 0)
        Log::info("- created {} quadrangles(s)",
                  utils::human_number(surface->quadrangles().size()));

    surface->set_meshed();
}

void
GeomModel::mesh_volume(ShapeID id)
{
    auto & volume = this->mvols_.at(id);
    mesh_volume(volume);
}

void
GeomModel::mesh_volume(Ptr<MeshVolume> volume)
{
    if (volume->is_meshed())
        Log::debug("Volume {} is already meshed", volume->id());

    auto & scheme = volume->scheme();

    auto surfaces = volume->surfaces();
    for (auto & srf : surfaces)
        scheme.select_surface_scheme(srf);
    for (auto & srf : surfaces)
        mesh_surface(srf);

    {
        auto & s = dynamic_cast<Scheme &>(scheme);
        auto pars_str = s.params_to_str();
        Log::info("Meshing volume {}: scheme='{}'{}",
                  volume->id(),
                  s.name(),
                  pars_str.empty() ? "" : fmt::format(", {}", pars_str));
        LoggingTimer timer;
        scheme.mesh_volume(volume);
    }
    volume->set_meshed();
}

void
GeomModel::set_block_name(Marker marker, const std::string & name)
{
    this->block_names_[marker] = name;
}

std::string
GeomModel::block_name(Marker marker) const
{
    try {
        return this->block_names_.at(marker);
    }
    catch (const std::out_of_range & e) {
        return "";
    }
}

void
GeomModel::set_side_set_name(Marker marker, const std::string & name)
{
    this->side_set_names_[marker] = name;
}

std::string
GeomModel::side_set_name(Marker marker) const
{
    try {
        return this->side_set_names_.at(marker);
    }
    catch (const std::out_of_range & e) {
        return "";
    }
}

void
GeomModel::set_node_set_name(Marker marker, const std::string & name)
{
    this->node_set_names_[marker] = name;
}

std::string
GeomModel::node_set_name(Marker marker) const
{
    try {
        return this->node_set_names_.at(marker);
    }
    catch (const std::out_of_range & e) {
        return "";
    }
}

ShapeID
GeomModel::get_shape_id(const TopoDS_Vertex & vertex)
{
    if (!this->vtx_id_.IsBound(vertex)) {
        ShapeID id = this->vtxs_.size() + 1;
        this->vtx_id_.Bind(vertex, id);
        return id;
    }
    else {
        return this->vtx_id_.Find(vertex);
    }
}

ShapeID
GeomModel::get_shape_id(const TopoDS_Edge & edge)
{
    if (!this->crv_id_.IsBound(edge)) {
        ShapeID id = this->crvs_.size() + 1;
        this->crv_id_.Bind(edge, id);
        return id;
    }
    else {
        return this->crv_id_.Find(edge);
    }
}

ShapeID
GeomModel::get_shape_id(const TopoDS_Face & face)
{
    if (!this->srf_id_.IsBound(face)) {
        ShapeID id = this->srfs_.size() + 1;
        this->srf_id_.Bind(face, id);
        return id;
    }
    else {
        return this->srf_id_.Find(face);
    }
}

ShapeID
GeomModel::get_shape_id(const TopoDS_Shell & shell)
{
    if (!this->vol_id_.IsBound(shell)) {
        ShapeID id = this->vols_.size() + 1;
        this->vol_id_.Bind(shell, id);
        return id;
    }
    else {
        return this->vol_id_.Find(shell);
    }
}

ShapeID
GeomModel::get_shape_id(const TopoDS_Solid & solid)
{
    if (!this->vol_id_.IsBound(solid)) {
        ShapeID id = this->vols_.size() + 1;
        this->vol_id_.Bind(solid, id);
        return id;
    }
    else {
        return this->vol_id_.Find(solid);
    }
}

} // namespace krado
