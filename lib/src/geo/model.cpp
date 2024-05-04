#include "krado/geo/model.h"
#include "krado/geo/vertex.h"
#include "krado/geo/curve.h"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"

namespace krado::geo {

Model::Model(const Shape & root_shape) : root_shape(root_shape)
{
    bind_shape(root_shape);
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
        this->curves[id] = new Curve(edge);
    }
}

void
Model::bind_vertices(const Shape & shape)
{
    TopExp_Explorer exp0;
    for (exp0.Init(shape, TopAbs_VERTEX); exp0.More(); exp0.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(exp0.Current());
        auto id = get_next_id();
        this->vertices[id] = new Vertex(vertex);
    }
}

int
Model::get_next_id()
{
    this->internal_id_counter++;
    return this->internal_id_counter;
}

} // namespace krado::geo
