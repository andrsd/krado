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
SchemeTriangulo::mesh_surface(MeshSurface & msurface)
{
    auto mcurves = msurface.curves();
    for (auto & crv : mcurves)
        select_meshing_scheme(*crv);

    for (auto & crv : mcurves)
        mesh().mesh_curve(*crv);
}

void
SchemeTriangulo::select_meshing_scheme(MeshCurve & curve)
{
    auto & pars = curve.meshing_parameters();
    auto scheme_name = pars.get<std::string>("scheme");
    if (scheme_name == "auto") {
        pars.set<std::string>("scheme") = "equal";
        pars.set<int>("intervals") = 1;
    }
}

} // namespace krado
