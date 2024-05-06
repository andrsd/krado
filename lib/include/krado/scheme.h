#pragma once

#include "krado/parameters.h"

namespace krado {

class Mesh;

class Scheme {
public:
    Scheme(Mesh & mesh, const Parameters & params);
    virtual ~Scheme() = default;

protected:
    Mesh & mesh();

private:
    Mesh & m;
};

} // namespace krado
