// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/trisurf.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_volume.h"
#include "krado/scheme2d.h"
#include "krado/range.h"
#include "krado/utils.h"
#include "BRepMesh_IncrementalMesh.hxx"
#include "BRep_Tool.hxx"
#include "TopoDS.hxx"
#include "Poly_Triangulation.hxx"
#include <array>
#include <map>
#include <cassert>

namespace krado {

static const std::string scheme_name = "trisurf";

SchemeTriSurf::SchemeTriSurf(Options options) :
    Scheme("trisurf"),
    Scheme3D(),
    Scheme2D(),
    Scheme1D(),
    opts_(options)
{
}

std::string
SchemeTriSurf::params_to_str()
{
    std::vector<std::string> spars;
    spars.push_back(fmt::format("linear_deflection={}", this->opts_.linear_deflection));
    spars.push_back(fmt::format("angular_deflection={}", this->opts_.angular_deflection));
    spars.push_back(fmt::format("is_relative={}", this->opts_.is_relative));
    return join(", ", spars);
}

void
SchemeTriSurf::mesh_volume(Ptr<MeshVolume> volume)
{
    auto lin_deflection = this->opts_.linear_deflection;
    auto angl_deflection = this->opts_.angular_deflection;
    auto is_relative = this->opts_.is_relative;

    auto shape = volume->geom_volume();

    Standard_Boolean is_rel = is_relative ? Standard_True : Standard_False;
    BRepMesh_IncrementalMesh mesh(shape, lin_deflection, is_rel, angl_deflection, true);

    std::map<Ptr<MeshCurve>, std::map<double, Ptr<MeshCurveVertex>>> curve_vertex_caches;

    for (auto & srf : volume->surfaces()) {
        assert(!srf.is_null());
        auto & geom_surface = srf->geom_surface();
        auto face = TopoDS::Face(geom_surface);

        TopLoc_Location location;
        auto triangulation = BRep_Tool::Triangulation(face, location);
        if (triangulation.IsNull())
            continue;

        srf->reserve_mem(triangulation->NbTriangles());
        std::map<Standard_Integer, Ptr<MeshVertexAbstract>> vertices;

        for (auto & curve : srf->curves()) {
            auto & geom_curve = curve->geom_curve();
            auto edge = TopoDS::Edge(geom_curve);
            auto polygon = BRep_Tool::PolygonOnTriangulation(edge, triangulation, location);
            if (polygon.IsNull())
                continue;

            auto bnd_vtxs = curve->bounding_vertices();

            // Build cache for this curve if not already done
            auto [it, inserted] = curve_vertex_caches.try_emplace(curve);
            auto & cache = it->second;
            if (inserted) {
                for (auto & cv : curve->curve_vertices())
                    cache.emplace(cv->parameter(), cv);
            }

            for (int j : make_range(polygon->NbNodes())) {
                auto idx = polygon->Node(j + 1);
                Ptr<MeshVertexAbstract> vtx;
                if (j == 0) {
                    vtx = bnd_vtxs[0];
                }
                else if (j == polygon->NbNodes() - 1) {
                    vtx = (bnd_vtxs.size() > 1) ? bnd_vtxs[1] : bnd_vtxs[0];
                }
                else {
                    auto u = polygon->Parameter(j + 1);
                    // Search for existing vertex with this parameter in cache
                    auto it = cache.lower_bound(u - 1e-9);
                    if (it != cache.end() && std::abs(it->first - u) < 1e-9) {
                        vtx = it->second;
                    }

                    if (vtx.is_null()) {
                        auto cv = Ptr<MeshCurveVertex>::alloc(geom_curve, u);
                        curve->add_vertex(cv);
                        cache.emplace(u, cv);
                        vtx = cv;
                    }
                }
                vertices.emplace(idx, vtx);
            }
        }

        for (auto i : make_range(triangulation->NbNodes())) {
            auto idx = i + 1;
            if (vertices.find(idx) != vertices.end())
                continue;

            auto uv = triangulation->UVNode(idx);
            auto vtx = Ptr<MeshSurfaceVertex>::alloc(geom_surface, uv.X(), uv.Y());
            srf->add_vertex(vtx);
            vertices.emplace(idx, vtx);
        }

        for (auto i : make_range(triangulation->NbTriangles())) {
            auto tri = triangulation->Triangle(i + 1);
            Standard_Integer n1, n2, n3;
            tri.Get(n1, n2, n3);
            std::array<Ptr<MeshVertexAbstract>, 3> t { vertices.at(n1),
                                                       vertices.at(n2),
                                                       vertices.at(n3) };
            srf->add_triangle(t);
        }
    }
}

void
SchemeTriSurf::mesh_surface(Ptr<MeshSurface> /*surface*/)
{
    // do nothing
}

void
SchemeTriSurf::mesh_curve(Ptr<MeshCurve> /*mcurve*/)
{
    // do nothing
}

void
SchemeTriSurf::select_surface_scheme(Ptr<MeshSurface> surface)
{
    if (!surface->has_scheme())
        surface->set_scheme<SchemeTriSurf>(this->opts_);
    else {
        auto scheme = dynamic_cast<SchemeTriSurf *>(&surface->scheme());
        if (scheme == nullptr) {
            auto s = dynamic_cast<Scheme *>(scheme);
            throw Exception("Unable to use {} in combination with scheme {}",
                            scheme_name,
                            s->name());
        }
    }
}

void
SchemeTriSurf::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        curve->set_scheme<SchemeTriSurf>(this->opts_);
    }
    else {
        auto scheme = dynamic_cast<SchemeTriSurf *>(&curve->scheme());
        if (scheme == nullptr) {
            auto s = dynamic_cast<Scheme *>(scheme);
            throw Exception("Unable to use {} in combination with scheme {}",
                            scheme_name,
                            s->name());
        }
    }
}

} // namespace krado
