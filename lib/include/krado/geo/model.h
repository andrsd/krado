#pragma once

#include "krado/geo/shape.h"
#include <map>

namespace krado::geo {

class Vertex;
class Curve;

class Model {
public:
    explicit Model(const Shape & root_shape);

private:
    void bind_shape(const Shape & shape);
    void bind_vertices(const Shape & shape);
    void bind_edges(const Shape & shape);
    void bind_faces(const Shape & shape);
    void bind_solids(const Shape & shape);

    int get_next_id();

    Shape root_shape;
    std::map<int, Vertex *> vertices;
    std::map<int, Curve *> curves;

    int internal_id_counter;
};

} // namespace krado::geo
