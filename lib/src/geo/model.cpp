#include "krado/geo/model.h"
#include "krado/geo/vertex.h"
#include "krado/geo/curve.h"
#include "krado/exception.h"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"

namespace krado::geo {

Model::Model(const Shape & root_shape) : root_shape(root_shape), internal_id_counter(0)
{
    bind_shape(root_shape);
}

const Vertex &
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
Model::vertex_id(const Vertex & vertex) const
{
    try {
        return this->shape_id.Find(vertex);
    }
    catch (...) {
        throw Exception("No ID for vertex");
    }
}

const Curve &
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
Model::curve_id(const Curve & curve) const
{
    try {
        return this->shape_id.Find(curve);
    }
    catch (...) {
        throw Exception("No ID for vertex");
    }
}

void
Model::bind_shape(const Shape & shape)
{
    bind_vertices(shape);
    bind_edges(shape);
    bind_faces(shape);
    bind_solids(shape);
}

void
Model::bind_solids(const Shape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_SOLID); exp0.More(); exp0.Next()) {
        TopoDS_Solid solid = TopoDS::Solid(exp0.Current());
    }
}

void
Model::bind_faces(const Shape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());
    }
}

void
Model::bind_edges(const Shape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_EDGE); exp0.More(); exp0.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp0.Current());
        auto id = get_next_id();
        this->crvs.emplace(id, Curve(edge));
        this->shape_id.Bind(edge, id);
    }
}

void
Model::bind_vertices(const Shape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_VERTEX); exp0.More(); exp0.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(exp0.Current());
        auto id = get_next_id();
        this->vtxs.emplace(id, Vertex(vertex));
        this->shape_id.Bind(vertex, id);
    }
}

int
Model::get_next_id()
{
    this->internal_id_counter++;
    return this->internal_id_counter;
}

} // namespace krado::geo
