#include "krado/mesh_surface.h"
#include "krado/exception.h"

namespace krado {

MeshSurface::MeshSurface(const GeomSurface & gsurface,
                         const std::vector<const MeshCurve *> & mesh_curves) :
    gsurface(gsurface),
    surface_marker(0),
    mesh_curves(mesh_curves)
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

const std::vector<const MeshCurve *> &
MeshSurface::curves() const
{
    return this->mesh_curves;
}

} // namespace krado
