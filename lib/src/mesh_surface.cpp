#include "krado/mesh_surface.h"
#include "krado/exception.h"

namespace krado {

MeshSurface::MeshSurface(const GeomSurface & gsurface) :
    gsurface(gsurface),
    surface_marker(0)
{
}

const GeomSurface &
MeshSurface::geom_surface() const
{
    return this->gsurface;
}

int
MeshSurface::marker() const
{
    return this->surface_marker;
}

void
MeshSurface::set_marker(int marker)
{
    this->surface_marker = marker;
}

} // namespace krado
