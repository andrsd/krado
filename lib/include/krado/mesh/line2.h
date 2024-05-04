#pragma once

#include "krado/mesh/element.h"

namespace krado::mesh {

class Line2 : public Element {
public:
    Line2(int pt0, int pt1);

private:
    int pt0, pt1;
};

} // namespace krado::mesh
