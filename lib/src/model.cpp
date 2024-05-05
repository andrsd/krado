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

const std::vector<GeomVertex> &
Model::vertices() const
{
    return this->vtxs;
}

const GeomVertex &
Model::vertex(int id) const
{
    try {
        auto index = this->vtx_index.at(id);
        return this->vtxs.at(index);
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

const std::vector<GeomCurve> &
Model::curves() const
{
    return this->crvs;
}

const GeomCurve &
Model::curve(int id) const
{
    try {
        auto index = this->crv_index.at(id);
        return this->crvs.at(index);
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

const std::vector<GeomSurface> &
Model::surfaces() const
{
    return this->srfs;
}

const GeomSurface &
Model::surface(int id) const
{
    try {
        auto index = this->srf_index.at(id);
        return this->srfs.at(index);
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
            auto index = this->srfs.size();
            this->srfs.emplace_back(GeomSurface(face));
            this->srf_index[id] = index;
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
            auto index = this->crvs.size();
            this->crvs.emplace_back(GeomCurve(edge));
            this->crv_index[id] = index;
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
            auto index = this->vtxs.size();
            this->vtxs.emplace_back(GeomVertex(vertex));
            this->vtx_index[id] = index;
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
