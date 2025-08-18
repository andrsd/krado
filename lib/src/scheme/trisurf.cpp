// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/trisurf.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/log.h"
#include "BRepMesh_IncrementalMesh.hxx"
#include "BRep_Tool.hxx"
#include "TopoDS.hxx"
#include "Poly_Triangulation.hxx"
#include <array>
#include <cassert>

namespace krado {

static const std::string scheme_name = "trisurf";

SchemeTriSurf::SchemeTriSurf() : Scheme(scheme_name), Scheme3D() {}

void
SchemeTriSurf::mesh_volume(Ptr<MeshVolume> volume)
{
    Log::info("Meshing volume {}: scheme='trisurf'", volume->id());

    auto lin_deflection = get<double>("linear_deflection");
    auto angl_deflection = get<double>("angular_deflection");
    auto is_relative = get<bool>("is_relative");

    const TopoDS_Shape & shape = volume->geom_volume();

    Standard_Boolean is_rel = is_relative ? Standard_True : Standard_False;
    BRepMesh_IncrementalMesh mesh(shape, lin_deflection, is_rel, angl_deflection, true);

    // process resulting mesh
    for (auto & srf : volume->surfaces()) {
        assert(!srf.is_null());
        auto & geom_surface = srf->geom_surface();
        auto face = TopoDS::Face(geom_surface);

        TopLoc_Location location;
        Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

        if (!triangulation.IsNull()) {
            srf->reserve_mem(triangulation->NbNodes(), triangulation->NbTriangles());

            for (int i = 0; i < triangulation->NbNodes(); ++i) {
                auto uv = triangulation->UVNode(i + 1);
                srf->add_vertex(Ptr<MeshSurfaceVertex>::alloc(geom_surface, uv.X(), uv.Y()));
            }

            auto & vertices = srf->all_vertices();
            for (int i = 0; i < triangulation->NbTriangles(); ++i) {
                auto tri = triangulation->Triangle(i + 1);
                Standard_Integer n1, n2, n3;
                tri.Get(n1, n2, n3);
                std::array<Ptr<MeshVertexAbstract>, 3> t { vertices[n1 - 1],
                                                           vertices[n2 - 1],
                                                           vertices[n3 - 1] };
                srf->add_triangle(t);
            }
        }
    }
}

void
SchemeTriSurf::mesh_surface(Ptr<MeshSurface> surface)
{
    // do nothing
}

void
SchemeTriSurf::mesh_curve(Ptr<MeshCurve> mcurve)
{
    // do nothing
}

void
SchemeTriSurf::select_surface_scheme(Ptr<MeshSurface> surface)
{
    if (surface->scheme()->name() != scheme_name) {
        if (surface->scheme()->name() == "auto") {
            surface->set_scheme(scheme_name);
        }
        else
            throw Exception("Unable to use {} in combination with scheme {}",
                            scheme_name,
                            surface->scheme()->name());
    }
}

void
SchemeTriSurf::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (curve->scheme()->name() != scheme_name) {
        if (curve->scheme()->name() == "auto") {
            curve->set_scheme(scheme_name);
        }
        else
            throw Exception("Unable to use {} in combination with scheme {}",
                            scheme_name,
                            curve->scheme()->name());
    }
}

} // namespace krado
