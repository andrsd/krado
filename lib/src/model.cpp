// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/model.h"
#include "krado/exception.h"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"

namespace krado {

Model::Model(const GeomShape & root_shape) : root_shape(root_shape), internal_id_counter(0)
{
    bind_shape(root_shape);
}

const std::map<int, GeomVertex> &
Model::vertices() const
{
    return this->vtxs;
}

const GeomVertex &
Model::vertex(int id) const
{
    try {
        return this->vtxs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No vertex with id {}", id);
    }
}

int
Model::vertex_id(const GeomVertex & vertex) const
{
    try {
        return this->shape_id.Find(vertex);
    }
    catch (...) {
        throw Exception("No ID for vertex");
    }
}

const std::map<int, GeomCurve> &
Model::curves() const
{
    return this->crvs;
}

const GeomCurve &
Model::curve(int id) const
{
    try {
        return this->crvs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No curve with id {}", id);
    }
}

int
Model::curve_id(const GeomCurve & curve) const
{
    try {
        return this->shape_id.Find(curve);
    }
    catch (...) {
        throw Exception("No ID for curve");
    }
}

const std::map<int, GeomSurface> &
Model::surfaces() const
{
    return this->srfs;
}

const GeomSurface &
Model::surface(int id) const
{
    try {
        return this->srfs.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No surface with id {}", id);
    }
}

int
Model::surface_id(const GeomSurface & surface) const
{
    try {
        return this->shape_id.Find(surface);
    }
    catch (...) {
        throw Exception("No ID for surface");
    }
}

const std::map<int, GeomVolume> &
Model::volumes() const
{
    return this->vols;
}

const GeomVolume &
Model::volume(int id) const
{
    try {
        return this->vols.at(id);
    }
    catch (std::out_of_range & e) {
        throw Exception("No volume with id {}", id);
    }
}

int
Model::volume_id(const GeomVolume & volume) const
{
    try {
        return this->shape_id.Find(volume);
    }
    catch (...) {
        throw Exception("No ID for volume");
    }
}

void
Model::bind_shape(const GeomShape & shape)
{
    bind_vertices(shape);
    bind_edges(shape);
    bind_faces(shape);
    bind_solids(shape);
}

void
Model::bind_solids(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_SOLID); exp0.More(); exp0.Next()) {
        TopoDS_Solid solid = TopoDS::Solid(exp0.Current());
        if (!this->shape_id.IsBound(solid)) {
            auto id = get_next_id();
            this->vols.emplace(id, GeomVolume(solid));
            this->shape_id.Bind(solid, id);
        }
    }
}

void
Model::bind_faces(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());
        if (!this->shape_id.IsBound(face)) {
            auto id = get_next_id();
            this->srfs.emplace(id, GeomSurface(face));
            this->shape_id.Bind(face, id);
        }
    }
}

void
Model::bind_edges(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_EDGE); exp0.More(); exp0.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp0.Current());
        if (!this->shape_id.IsBound(edge)) {
            auto id = get_next_id();
            this->crvs.emplace(id, GeomCurve(edge));
            this->shape_id.Bind(edge, id);
        }
    }
}

void
Model::bind_vertices(const GeomShape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_VERTEX); exp0.More(); exp0.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(exp0.Current());
        if (!this->shape_id.IsBound(vertex)) {
            auto id = get_next_id();
            this->vtxs.emplace(id, GeomVertex(vertex));
            this->shape_id.Bind(vertex, id);
        }
    }
}

int
Model::get_next_id()
{
    this->internal_id_counter++;
    return this->internal_id_counter;
}

} // namespace krado
