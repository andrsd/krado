#include "krado/scheme.h"
#include "krado/mesh.h"

namespace krado {

Scheme::Scheme(Mesh & mesh, const Parameters & params) : m(mesh) {}

Mesh &
Scheme::mesh()
{
    return this->m;
}

} // namespace krado
