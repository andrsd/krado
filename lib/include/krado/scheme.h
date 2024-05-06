#pragma once

#include "krado/parameters.h"

namespace krado {

class Scheme {
public:
    Scheme(const Parameters & params) {}
    virtual ~Scheme() = default;
};

} // namespace krado
