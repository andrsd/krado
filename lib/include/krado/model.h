#pragma once

#include "krado/geom_shape.h"
#include "krado/geom_vertex.h"
#include "krado/geom_curve.h"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include <map>

namespace krado {

class Model {
public:
    explicit Model(const GeomShape & root_shape);

    /// Get model vertices
    ///
    /// @return Vertices
    const std::map<int, GeomVertex> & vertices() const;

    /// Get vertex with specified ID
    ///
    /// @param id Vertex id
    /// @return Vertex with specified ID
    const GeomVertex & vertex(int id) const;

    /// Get vertex ID
    ///
    /// @param vertex Vertex
    /// @return Vertex ID
    int vertex_id(const GeomVertex & vertex) const;

    /// Get model curves
    ///
    /// @return Curves
    const std::map<int, GeomCurve> & curves() const;

    /// Get curve with specified ID
    ///
    /// @param id Curve id
    /// @return Curve with specified ID
    const GeomCurve & curve(int id) const;

    /// Get curve ID
    ///
    /// @param curve Curve
    /// @return Curve ID
    int curve_id(const GeomCurve & curve) const;

private:
    void bind_shape(const GeomShape & shape);
    void bind_vertices(const GeomShape & shape);
    void bind_edges(const GeomShape & shape);
    void bind_faces(const GeomShape & shape);
    void bind_solids(const GeomShape & shape);

    int get_next_id();

    GeomShape root_shape;
    std::map<int, GeomVertex> vtxs;
    std::map<int, GeomCurve> crvs;
    TopTools_DataMapOfShapeInteger shape_id;
    int internal_id_counter;
};

} // namespace krado
