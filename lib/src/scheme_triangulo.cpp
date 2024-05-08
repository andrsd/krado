#include "krado/scheme_triangulo.h"
#include "krado/mesh.h"

namespace krado {

SchemeTriangulo::SchemeTriangulo(Mesh & mesh, const Parameters & params) :
    Scheme(mesh, params)
{
    if (params.has<std::vector<Point>>("holes"))
        this->holes = params.get<std::vector<Point>>("holes");
}

void
SchemeTriangulo::mesh_surface(MeshSurface & surface)
{
}

} // namespace krado
