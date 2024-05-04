#pragma once

#include "TopoDS_Vertex.hxx"

namespace krado::geo {

class Vertex {
public:
    explicit Vertex(const TopoDS_Vertex & vertex);

private:
    TopoDS_Vertex vertex;
    double x, y, z;
};

} // namespace krado::geo
