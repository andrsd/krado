#include "krado/scheme_triangulo.h"

namespace krado {

SchemeTriangulo::SchemeTriangulo(const Parameters & params) :
    Scheme(params)
{
    if (params.has<std::vector<Point>>("holes"))
        this->holes = params.get<std::vector<Point>>("holes");
}

void
SchemeTriangulo::mesh_surface(MeshSurface & msurface)
{
}

} // namespace krado
