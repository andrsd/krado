// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_model.h"
#include "krado/exception.h"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"

namespace krado {

GeomModel::GeomModel(const GeomShape & root_shape) : root_shape(root_shape)
{
    bind_shape(root_shape);
}

const std::map<int, GeomVertex> &
GeomModel::vertices() const
{
    return this->vtxs;
}

const GeomVertex &
GeomModel::vertex(int id) const
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

const std::map<int, GeomCurve> &
GeomModel::curves() const
{
    return this->crvs;
}

const GeomCurve &
GeomModel::curve(int id) const
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

const std::map<int, GeomSurface> &
GeomModel::surfaces() const
{
    return this->srfs;
}

const GeomSurface &
GeomModel::surface(int id) const
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

const std::map<int, GeomVolume> &
GeomModel::volumes() const
{
    return this->vols;
}

const GeomVolume &
GeomModel::volume(int id) const
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

} // namespace krado
