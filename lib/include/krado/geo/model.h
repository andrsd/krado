#pragma once

#include "TopoDS_Shape.hxx"

namespace krado {

class Model {
public:
    Model(const TopoDS_Shape & root_shape);

private:
    TopoDS_Shape root_shape;
};

} // namespace krado
