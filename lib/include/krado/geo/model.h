#pragma once

#include "krado/geo/shape.h"
#include "krado/geo/vertex.h"
#include "krado/geo/curve.h"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include <map>

namespace krado::geo {

class Model {
public:
    explicit Model(const Shape & root_shape);

    /// Get vertex with specified ID
    ///
    /// @param id Vertex id
    /// @return Vertex with specified ID
    const Vertex & vertex(int id) const;

    /// Get vertex ID
    ///
    /// @param vertex Vertex
    /// @return Vertex ID
    int vertex_id(const Vertex & vertex) const;

    /// Get curve with specified ID
    ///
    /// @param id Curve id
    /// @return Curve with specified ID
    const Curve & curve(int id) const;

    /// Get curve ID
    ///
    /// @param curve Curve
    /// @return Curve ID
    int curve_id(const Curve & curve) const;

private:
    void bind_shape(const Shape & shape);
    void bind_vertices(const Shape & shape);
    void bind_edges(const Shape & shape);
    void bind_faces(const Shape & shape);
    void bind_solids(const Shape & shape);

    int get_next_id();

    Shape root_shape;
    std::map<int, Vertex> vtxs;
    std::map<int, Curve> crvs;
    TopTools_DataMapOfShapeInteger shape_id;
    int internal_id_counter;
};

} // namespace krado::geo
