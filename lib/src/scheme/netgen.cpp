// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/netgen.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/log.h"
#include "occgeom.hpp"
#include <map>

namespace krado {

static const std::string scheme_name = "netgen";

SchemeNetgen::SchemeNetgen(Options options) :
    Scheme3D(scheme_name),
    Scheme2D(scheme_name),
    Scheme1D(scheme_name),
    opts_(options)
{
}

void
SchemeNetgen::mesh_volume(Ptr<MeshVolume> volume)
{
    Log::info("Meshing volume {}: scheme='{}'", volume->id(), scheme_name);
}

void
SchemeNetgen::mesh_surface(Ptr<MeshSurface> surface)
{
    Log::info("Meshing surface {}: scheme='{}'", surface->id(), scheme_name);
}

void
SchemeNetgen::mesh_curve(Ptr<MeshCurve> curve)
{
    Log::info("Meshing curve {}: scheme='{}'", curve->id(), scheme_name);

    auto & gcrv = curve->geom_curve();

    netgen::MeshingParameters mparam;
    mparam.uselocalh = true;
    mparam.segmentsperedge = 10.0;
    mparam.curvaturesafety = 2.0;
    mparam.maxh = 0.5;
    mparam.minh = 0.01;
    mparam.grading = this->opts_.grading;

    auto occgeo = std::make_shared<netgen::OCCGeometry>();
    occgeo->shape = gcrv;
    occgeo->changed = 1;
    occgeo->BuildFMap();
    occgeo->CalcBoundingBox();

    netgen::Mesh mesh;
    mesh.SetGeometry(occgeo);
    mesh.geomtype = netgen::Mesh::GEOM_OCC;

    netgen::Box<3> bb = occgeo->GetBoundingBox();
    bb.Increase(bb.Diam() / 10);
    mesh.SetLocalH(bb.PMin(), bb.PMax(), this->opts_.grading, 1);

    auto [u_lo, u_hi] = gcrv.param_range();
    double du = (u_hi - u_lo) / 40.;
    for (double u = u_lo; u <= u_hi + 1e-9; u += du) {
        auto h = curve->mesh_size_at_param(u);
        if (h > 0.5)
            h = 0.5;
        auto pt = gcrv.point(u);
        mesh.RestrictLocalH(netgen::Point3d(pt.x, pt.y, pt.z), h);
    }

    occgeo->FindEdges(mesh, mparam);

    // first add boundary vertices, they are "meshed" already
    std::map<Point, Ptr<MeshVertexAbstract>> surf_idx;
    for (auto & v : curve->bounding_vertices()) {
        auto pt = v->point();
        auto [it, inserted] = surf_idx.try_emplace(pt, v);
        if (inserted)
            curve->add_vertex(v);
    }

    // add internal (curve) vertices
    for (netgen::PointIndex i = 1; i <= mesh.GetNP(); i++) {
        netgen::Point3d p = mesh.Point(i);
        Point pt(p.X(), p.Y(), p.Z());
        auto u = gcrv.parameter_from_point(pt);
        auto vtx = Ptr<MeshCurveVertex>::alloc(gcrv, u);
        auto [it, inserted] = surf_idx.try_emplace(pt, vtx);
        if (inserted)
            curve->add_vertex(vtx);
    }

    // build segments
    auto & celems = mesh.LineSegments();
    for (auto & e : celems) {
        std::array<int, 2> idx = { e.pnums[0].i, e.pnums[1].i };
        netgen::Point3d p1 = mesh.Point(idx[0]);
        netgen::Point3d p2 = mesh.Point(idx[1]);

        Point pt1(p1.X(), p1.Y(), p1.Z());
        Point pt2(p2.X(), p2.Y(), p2.Z());

        auto it1 = surf_idx.find(pt1);
        auto it2 = surf_idx.find(pt2);
        if ((it1 != surf_idx.end()) and (it2 != surf_idx.end())) {
            curve->add_segment({ it1->second, it2->second });
        }
        else
            throw Exception("Internal error: points were not found");
    }
}

void
SchemeNetgen::select_surface_scheme(Ptr<MeshSurface> surface)
{
    if (!surface->has_scheme())
        surface->set_scheme<SchemeNetgen>(this->opts_);
}

void
SchemeNetgen::select_curve_scheme(Ptr<MeshCurve> curve)
{
    if (!curve->has_scheme()) {
        curve->set_scheme<SchemeNetgen>(this->opts_);
    }
}

} // namespace krado
