#include "krado/mesh_surface.h"
#include "krado/exception.h"

namespace krado {

MeshSurface::MeshSurface(const GeomSurface & gsurface,
                         const std::vector<MeshCurve *> & mesh_curves) :
    gsurface(gsurface),
    mesh_curves(mesh_curves),
    meshed(false)
{
    auto & mpars = meshing_parameters();
    mpars.set<std::string>("scheme") = "auto";
    mpars.set<int>("marker") = 0;
}

const GeomSurface &
MeshSurface::geom_surface() const
{
    return this->gsurface;
}

const std::vector<MeshCurve *> &
MeshSurface::curves() const
{
    return this->mesh_curves;
}

bool
MeshSurface::is_meshed() const
{
    return this->meshed;
}

void
MeshSurface::set_meshed()
{
    this->meshed = true;
}

} // namespace krado
