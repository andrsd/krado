#pragma once

#include "krado/geo/shape.h"

namespace krado {

class Model {
public:
    Model(const Shape & root_shape);

private:
    Shape root_shape;
};

} // namespace krado
