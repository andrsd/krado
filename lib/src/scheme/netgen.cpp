// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/netgen.h"
#include "krado/mesh_volume.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/surface_index_mapper.h"
#include "krado/log.h"
#include "krado/uv_param.h"
#include "occgeom.hpp"
#include <map>

namespace krado {

static const std::string scheme_name = "netgen";

namespace {

struct PointTree {
    std::map<int, netgen::BoxTree<3>> tree;
    netgen::Box<3> bounding_box;

    PointTree(netgen::Box<3> bb) : bounding_box(bb) {}

    void
    Insert(netgen::Point<3> p, netgen::PointIndex n, int index)
    {
        if (tree.count(index) == 0)
            tree.emplace(index, bounding_box);
        tree.at(index).Insert(p, p, n);
    }

    netgen::PointIndex
    Find(netgen::Point<3> p, int index) const
    {
        netgen::ArrayMem<int, 1> points;
        tree.at(index).GetIntersecting(p, p, points);
        if (points.Size() == 0)
            throw Exception("cannot find mapped point {}", netgen::ToString(p));
        return points[0];
    }
};

inline void
NOOP_Deleter(void *)
{
}

} // namespace

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

    auto & gsurf = surface->geom_surface();
    auto occgeo = std::make_shared<netgen::OCCGeometry>();
    occgeo->shape = gsurf;
    occgeo->changed = 1;
    occgeo->BuildFMap();
    occgeo->CalcBoundingBox();

    netgen::Mesh mesh;
    mesh.SetGeometry(occgeo);
    mesh.geomtype = netgen::Mesh::GEOM_OCC;

    netgen::MeshingParameters mparam;
    mparam.uselocalh = 1;
    mparam.segmentsperedge = 2.0;
    mparam.curvaturesafety = 2.0;
    mparam.minh = 0.1;
    mparam.maxh = 0.25;
    mparam.grading = this->opts_.grading;

    netgen::Box<3> bb = occgeo->GetBoundingBox();
    bb.Increase(bb.Diam() / 10);
    mesh.SetLocalH(bb.PMin(), bb.PMax(), this->opts_.grading, 1);

    // Add mesh size restrictions from surface
    auto [u_lo, u_hi] = gsurf.param_range(0);
    auto [v_lo, v_hi] = gsurf.param_range(1);
    int nu = 6, nv = 6;
    for (int i = 0; i < nu; i++) {
        for (int j = 0; j < nv; j++) {
            double u = u_lo + (i + 0.5) * (u_hi - u_lo) / nu;
            double v = v_lo + (j + 0.5) * (v_hi - v_lo) / nv;
            UVParam uv(u, v);
            auto h = surface->mesh_size_at_param(uv);
            auto pt = gsurf.point(uv);
            mesh.RestrictLocalH(netgen::Point3d(pt.x, pt.y, pt.z), h);
        }
    }

    // FindEdges
    PointTree tree(bb);

    auto & identifications = mesh.GetIdentifications();

    auto vertices = occgeo->Vertices();
    auto edges = occgeo->Edges();

    netgen::Array<netgen::PointIndex> vert2meshpt(vertices.Size());
    vert2meshpt = netgen::PointIndex::INVALID;

    for (auto & vert : vertices) {
        auto pi = mesh.AddPoint(vert->GetPoint(), vert->properties.layer);
        vert2meshpt[vert->nr] = pi;
        mesh[pi].Singularity(vert->properties.hpref);
        mesh[pi].SetType(netgen::FIXEDPOINT);

        netgen::Element0d el(pi, pi - netgen::IndexBASE<netgen::PointIndex>() + 1);
        el.name = vert->properties.GetName();
        mesh.SetCD3Name(pi - netgen::IndexBASE<netgen::PointIndex>() + 1, el.name);
        mesh.pointelements.Append(el);
    }

    for (auto & vert : vertices)
        for (auto & ident : vert->identifications)
            identifications.Add(vert2meshpt[ident.from->nr],
                                vert2meshpt[ident.to->nr],
                                ident.name,
                                ident.type);

    // size_t segnr = 0;
    auto nedges = edges.Size();
    netgen::Array<netgen::Array<netgen::PointIndex>> all_pnums(nedges);
    netgen::Array<netgen::Array<double>> all_params(nedges);

    for (auto edgenr : Range(edges)) {
        auto edge = edges[edgenr].get();
        netgen::PointIndex startp, endp;
        // throws if points are not found
        startp = vert2meshpt[edge->GetStartVertex().nr];
        endp = vert2meshpt[edge->GetEndVertex().nr];

        // ignore collapsed edges
        if (edge->IsDegenerated())
            continue;

        // ----------- Add Points to mesh and create segments -----
        auto & pnums = all_pnums[edgenr];
        auto & params = all_params[edgenr];
        netgen::Array<netgen::Point<3>> edge_points;
        netgen::Array<double> edge_params;

        if (edge->primary == edge) {
            // check if start and end vertex are identified (if so, we only insert one segment and
            // do z-refinement later)
            bool is_identified_edge = false;
            auto v0 = vertices[edge->GetStartVertex().nr].get();
            auto v1 = vertices[edge->GetEndVertex().nr].get();
            for (auto & ident : v0->identifications) {
                auto other = ident.from == v0 ? ident.to : ident.from;
                if (other->nr == v1->nr && ident.type == netgen::Identifications::CLOSESURFACES) {
                    is_identified_edge = true;
                    break;
                }
            }

            if (is_identified_edge) {
                params.SetSize(2);
                params[0] = 0.;
                params[1] = 1.;
            }
            else {
                edge->Divide(mparam, mesh, edge_points, params);
            }
        }
        else {
            auto nr_primary = edge->primary->nr;
            auto & pnums_primary = all_pnums[nr_primary];
            // auto & params_primary = all_params[nr_primary];
            auto trafo = edge->primary_to_me;

            auto np = pnums_primary.Size();
            edge_points.SetSize(np - 2);
            edge_params.SetSize(np - 2);
            for (auto i : netgen::Range(np - 2)) {
                edge_points[i] = mesh[pnums_primary[i + 1]];
                if (trafo)
                    edge_points[i] = (*trafo)(edge_points[i]);
                netgen::EdgePointGeomInfo gi;
                edge->ProjectPoint(edge_points[i], &gi);
                edge_params[i] = gi.dist;
            }

            params.SetSize(edge_params.Size() + 2);

            for (auto i : Range(edge_params))
                params[i + 1] = edge_params[i];

            if (edge_params.Size() > 1) {
                // Just projecting (code below) does not work for closed edges (startp == endp)
                // In this case, there are at least 2 inner points which we use to check edge
                // orientation
                bool reversed = edge_params[1] < edge_params[0];
                if (reversed) {
                    params[0] = 1.0;
                    params.Last() = 0.0;
                }
                else {
                    params.Last() = 1.0;
                    params[0] = 0.0;
                }
            }
            else {
                for (size_t i : std::vector { 0UL, pnums_primary.Size() - 1 }) {
                    auto p_mapped = mesh[pnums_primary[i]];
                    if (trafo)
                        p_mapped = (*trafo)(p_mapped);
                    netgen::EdgePointGeomInfo gi;
                    edge->ProjectPoint(p_mapped, &gi);
                    params[i] = gi.dist;
                }
            }
        }

        pnums.SetSize(edge_points.Size() + 2);

        bool is_reversed = params.Last() < params[0];
        pnums[0] = is_reversed ? endp : startp;
        pnums.Last() = is_reversed ? startp : endp;

        for (auto i : Range(edge_points)) {
            auto pi = mesh.AddPoint(edge_points[i], edge->properties.layer);
            if (edge->identifications.Size())
                tree.Insert(mesh[pi], pi, edge->nr);
            pnums[i + 1] = pi;
        }

        for (auto i : netgen::Range(pnums.Size() - 1)) {
            // segnr++;
            netgen::Segment seg;
            seg[0] = pnums[i];
            seg[1] = pnums[i + 1];
            seg.edgenr = edgenr + 1;
            seg.si = edgenr + 1;
            seg.epgeominfo[0].dist = params[i];
            seg.epgeominfo[1].dist = params[i + 1];
            seg.epgeominfo[0].edgenr = edgenr;
            seg.epgeominfo[1].edgenr = edgenr;
            seg.singedge_left = edge->properties.hpref;
            seg.singedge_right = edge->properties.hpref;
            seg.domin = edge->domin + 1;
            seg.domout = edge->domout + 1;
            mesh.AddSegment(seg);
        }
        mesh.SetCD2Name(edgenr + 1, edge->properties.GetName());
    }

    for (auto & edge : edges) {
        // identify points on edge
        for (auto & ident : edge->identifications)
            if (ident.from == edge.get()) {
                auto & pnums = all_pnums[edge->nr];
                if (pnums.Size() < 2)
                    continue; // degenerated edge
                // start and end vertex are already identified
                for (auto pi : pnums.Range(1, pnums.Size() - 1)) {
                    netgen::Point<3> p_other = mesh[pi];
                    if (ident.trafo)
                        p_other = (*ident.trafo)(mesh[pi]);
                    else
                        static_cast<netgen::GeometryEdge *>(ident.to)->ProjectPoint(p_other,
                                                                                    nullptr);
                    auto pi_other = tree.Find(p_other, ident.to->nr);
                    identifications.Add(pi, pi_other, ident.name, ident.type);
                }
            }
    }
    mesh.CalcSurfacesOfNode();

    // end of findedges()

    // Now mesh the surface using netgen
    occgeo->MeshSurface(mesh, mparam);
    mesh.CalcSurfacesOfNode();

    // Add triangles/quads to surface
    SurfaceIndexMapper im(surface);
    for (const auto & el : mesh.SurfaceElements()) {
        if (el.GetType() == netgen::TRIG) {
            std::array<Ptr<MeshVertexAbstract>, 3> tri;
            for (int i = 0; i < 3; i++) {
                netgen::Point3d p = mesh.Point(el[i]);
                tri[i] = im.surface_vertex(p.X(), p.Y());
            }
            surface->add_triangle(tri);
        }
        else if (el.GetType() == netgen::QUAD) {
            std::array<Ptr<MeshVertexAbstract>, 4> quad;
            for (int i = 0; i < 4; i++) {
                netgen::Point3d p = mesh.Point(el[i]);
                quad[i] = im.surface_vertex(p.X(), p.Y());
            }
            surface->add_quadrangle(quad);
        }
    }
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
