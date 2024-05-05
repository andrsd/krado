#pragma once

#include "TopoDS_Vertex.hxx"

namespace krado::geo {

class Vertex {
public:
    explicit Vertex(const TopoDS_Vertex & vertex);

    double x() const;
    double y() const;
    double z() const;

    operator const TopoDS_Shape &() const;

private:
    TopoDS_Vertex vertex;
    double x_coord, y_coord, z_coord;
};

} // namespace krado::geo
