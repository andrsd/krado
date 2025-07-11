#include "krado/scheme/frontal_delaunay.h"
#include "krado/mesh.h"
#include "krado/mesh_element.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_triangulation.h"
#include "krado/bds.h"
#include "krado/poly_mesh.h"
#include "krado/bounding_box_3d.h"
#include "krado/consts.h"
#include "krado/scheme/frontal_delaunay_insertion.h"
#include "krado/scheme/frontal_delaunay_optimize.h"
#include "krado/scheme/frontal_delaunay_bds.h"
#include "krado/geom_surface.h"
#include "krado/surface_index_mapper.h"
#include "krado/log.h"
#include <array>
#include <iostream>

namespace krado {

namespace {

void
convert_from_bds(BDS_Mesh & m,
                 MeshSurface & msurface,
                 std::map<BDS_Point *, MeshVertexAbstract *, PointLessThan> & recover_map)
{
    for (auto & t : m.triangles) {
        assert(t != nullptr);
        if (!t->deleted) {
            BDS_Point * n[4];
            t->get_nodes(n);
            std::array<MeshVertexAbstract *, 4> v { nullptr, nullptr, nullptr, nullptr };
            for (int i = 0; i < 4; ++i) {
                if (!n[i])
                    continue;
                if (recover_map.find(n[i]) == recover_map.end()) {
                    auto sv = new MeshSurfaceVertex(msurface.geom_surface(), n[i]->u, n[i]->v);
                    v[i] = sv;
                    recover_map[n[i]] = v[i];
                    msurface.add_vertex(sv);
                }
                else
                    v[i] = recover_map[n[i]];
            }
            if (v[3] == nullptr) {
                // when a singular point is present, degenerated triangles may be
                // created, for example on a sphere that contains one pole
                if (v[0] != v[1] && v[0] != v[2] && v[1] != v[2]) {
                    assert(v[0] != nullptr);
                    assert(v[1] != nullptr);
                    assert(v[0] != nullptr);
                    msurface.add_triangle({ v[0], v[1], v[2] });
                }
            }
            // else
            //     msurface.add_element(MeshElement::Quad4({ v[0], v[1], v[2], v[3] }));
        }
    }
}

bool
recover_curve(BDS_Mesh & m,
              MeshSurface & msurface,
              MeshCurve * mcurve,
              std::map<MeshVertexAbstract *, BDS_Point *> & recover_map_inv,
              std::set<EdgeToRecover> & e2r,
              std::set<EdgeToRecover> & not_recovered,
              int pass)
{
    assert(mcurve != nullptr);

    BDS_GeomEntity * g = nullptr;
    if (pass == 2) {
        auto tag = mcurve->id();
        m.add_geom(tag, 1);
        g = m.get_geom(tag, 1);
    }

    for (std::size_t i = 0; i < mcurve->segments().size(); ++i) {
        auto & seg = mcurve->segments()[i];
        auto * vstart = seg.vertex(0);
        auto * vend = seg.vertex(1);
        auto itpstart = recover_map_inv.find(vstart);
        auto itpend = recover_map_inv.find(vend);
        if (itpstart != recover_map_inv.end() && itpend != recover_map_inv.end()) {
            auto * pstart = itpstart->second;
            auto * pend = itpend->second;
            assert(pstart != nullptr);
            assert(pend != nullptr);
            if (pass == 1)
                e2r.insert({ pstart->iD, pend->iD, &mcurve->geom_curve() });
            else {
                auto [e, failed] = m.recover_edge(pstart->iD, pend->iD, e2r, not_recovered);
                if (e)
                    e->g = g;
                else {
                    if (failed) {
                        throw Exception(
                            "Unable to recover the edge {} ({}/{}) on curve {} (on surface {})",
                            -1, // seg.getNum(),
                            i + 1,
                            mcurve->segments().size(),
                            mcurve->id(),
                            msurface.id());
                    }
                    return !failed;
                }
            }
        }
    }

    auto & bnd_verts = mcurve->bounding_vertices();
    if (pass == 2 && bnd_verts[0]) {
        auto itpstart = recover_map_inv.find(bnd_verts[0]);
        auto itpend = recover_map_inv.find(bnd_verts[1]);
        if (itpstart != recover_map_inv.end() && itpend != recover_map_inv.end()) {
            auto * pstart = itpstart->second;
            auto * pend = itpend->second;
            assert(pstart != nullptr);
            assert(pend != nullptr);
            if (!pstart->g) {
                m.add_geom(pstart->iD, 0);
                auto * g0 = m.get_geom(pstart->iD, 0);
                pstart->g = g0;
            }
            if (!pend->g) {
                m.add_geom(pend->iD, 0);
                auto * g0 = m.get_geom(pend->iD, 0);
                pend->g = g0;
            }
        }
    }

    return true;
}

void
remesh_unrecovered_edges(std::map<MeshVertexAbstract *, BDS_Point *> & recoverMapInv,
                         std::set<EdgeToRecover> & edgesNotRecovered)
{
    throw Exception("remesh_unrecovered_edges not implemented yet");
}

void
delete_unused_vertices(MeshSurface & msurface)
{
    std::set<MeshSurfaceVertex *, MeshVertexAbstract::PtrLessThan> all_verts;
    for (auto & el : msurface.elements()) {
        for (int j = 0; j < el.num_vertices(); ++j) {
            assert(el.vertex(j) != nullptr);
            if ((TopoDS_Shape) el.vertex(j)->geom_shape() ==
                (TopoDS_Shape) msurface.geom_surface()) {
                auto * sv = dynamic_cast<MeshSurfaceVertex *>(el.vertex(j));
                assert(sv != nullptr);
                all_verts.insert(sv);
            }
        }
    }

    for (auto & vtx : msurface.surface_vertices()) {
        if (all_verts.find(vtx) == all_verts.end())
            delete vtx;
    }
    auto & verts = msurface.surface_vertices();
    verts.clear();
    verts.insert(verts.end(), all_verts.begin(), all_verts.end());
}

bool
mesh_generator(MeshSurface & msurface,
               int recur_iter,
               bool repair_self_intersecting_1d_mesh,
               bool only_initial_mesh,
               bool debug)
{
    auto & gsurface = msurface.geom_surface();

    // ---
    BDS_GeomEntity CLASS_F(1, 2);
    BDS_GeomEntity CLASS_EXTERIOR(1, 3);
    std::map<BDS_Point *, MeshVertexAbstract *, PointLessThan> recover_map;
    std::map<MeshVertexAbstract *, BDS_Point *> recover_map_inv;

    // build a set with all points of the boundaries
    std::set<MeshVertexAbstract *, MeshVertexAbstract::PtrLessThan> all_vertices, boundary;
    for (auto & e : msurface.curves()) {
        assert(e != nullptr);
        if (e->geom_curve().is_seam(gsurface))
            return false;

        if (!e->is_mesh_degenerated()) {
            for (auto & seg : e->segments()) {
                auto * v1 = seg.vertex(0);
                auto * v2 = seg.vertex(1);
                all_vertices.insert(v1);
                all_vertices.insert(v2);
                if (boundary.find(v1) == boundary.end())
                    boundary.insert(v1);
                else
                    boundary.erase(v1);
                if (boundary.find(v2) == boundary.end())
                    boundary.insert(v2);
                else
                    boundary.erase(v2);
            }
        }
        else
            Log::debug("Degenerated mesh on edge {}", e->id());
    }

    if (boundary.size()) {
        throw Exception("The 1D mesh seems not to be forming a closed loop ({} boundary nodes are "
                        "considered once)",
                        boundary.size());
    }

    // add embedded curves
    for (auto & mcrv : msurface.embedded_curves()) {
        assert(mcrv != nullptr);
        if (!mcrv->is_mesh_degenerated()) {
            all_vertices.insert(mcrv->curve_vertices().begin(), mcrv->curve_vertices().end());
            auto & bnd_verts = mcrv->bounding_vertices();
            if (bnd_verts[0])
                all_vertices.insert(bnd_verts[0]);
            if (bnd_verts[1])
                all_vertices.insert(bnd_verts[1]);
        }
    }
    // add embedded vertices
    for (auto & mvtx : msurface.embedded_vertices())
        all_vertices.insert(mvtx);

    if (all_vertices.size() < 3) {
        Log::warn("Mesh generation of surface %d skipped: only {} nodes on the boundary",
                  msurface.id(),
                  all_vertices.size());
        // gf->meshStatistics.status = GFace::DONE;
        return true;
    }
    else if (all_vertices.size() == 3) {
        std::array<MeshVertexAbstract *, 3> tri;
        std::copy(all_vertices.begin(), all_vertices.end(), tri.begin());
        msurface.add_triangle(tri);
        return true;
    }

    // Buid a BDS_Mesh structure that is convenient for doing the actual meshing procedure
    BDS_Mesh m;

    std::vector<BDS_Point *> points;
    points.reserve(all_vertices.size());
    BoundingBox3D bbox;
    for (auto & vtx : all_vertices) {
        assert(vtx != nullptr);
        auto & ge = vtx->geom_shape();
        auto [par, success] = reparam_mesh_vertex_on_surface(vtx, msurface.geom_surface());
        auto * pp = m.add_point(points.size(), par, msurface.geom_surface());
        m.add_geom(ge.id(), ge.dim());
        BDS_GeomEntity * g = m.get_geom(ge.id(), ge.dim());
        assert(pp != nullptr);
        pp->g = g;
        recover_map[pp] = vtx;
        recover_map_inv[vtx] = pp;
        points.push_back(pp);
        bbox += Point(par.u, par.v, 0);
    }

    bbox.make_cube();

    {
        // std::vector<GEdge *> temp;
        // if(replacementEdges) {
        //   temp = gf->edges();
        //   gf->set(*replacementEdges);
        // }

        // recover and color so most of the code below can go away. Works also for
        // periodic faces
        int gf_tag = 0;
        auto pm = surface_initial_mesh(msurface, true);

        std::map<int, BDS_Point *> aaa;
        for (auto & vtx : all_vertices)
            aaa[vtx->num()] = recover_map_inv[vtx];

        for (int ip = 0; ip < 4; ++ip) {
            auto * v = pm.vertices[ip];
            assert(v != nullptr);
            v->data = -ip - 1;
            auto gsurf = msurface.geom_surface();
            auto * pp = m.add_point(v->data, { v->position.x, v->position.y }, gsurf);
            m.add_geom(gf_tag, 2);
            auto * g = m.get_geom(gf_tag, 2);
            assert(pp != nullptr);
            pp->g = g;
            aaa[v->data] = pp;
        }

        for (auto & face : pm.faces) {
            assert(face != nullptr);
            auto * he = face->he;
            assert(he != nullptr);
            assert(he->v != nullptr);
            int a = he->v->data;
            assert(he->next != nullptr);
            assert(he->next->v != nullptr);
            int b = he->next->v->data;
            assert(he->next->next != nullptr);
            assert(he->next->next->v != nullptr);
            int c = he->next->next->v->data;
            auto * p1 = aaa[a];
            auto * p2 = aaa[b];
            auto * p3 = aaa[c];
            if (p1 && p2 && p3)
                m.add_triangle(p1->iD, p2->iD, p3->iD);
        }
    }

    // Recover the boundary edges and compute characteristic lenghts using mesh
    // edge spacing. If two of these edges intersect, then the 1D mesh have to be
    // densified
    Log::debug("Recovering {} model curves", msurface.curves().size());
    std::set<EdgeToRecover> edges_to_recover;
    std::set<EdgeToRecover> edges_not_recovered;
    for (auto & mcrv : msurface.curves()) {
        assert(mcrv != nullptr);
        if (!mcrv->is_mesh_degenerated())
            recover_curve(m,
                          msurface,
                          mcrv,
                          recover_map_inv,
                          edges_to_recover,
                          edges_not_recovered,
                          1);
    }
    for (auto & mcrv : msurface.embedded_curves()) {
        assert(mcrv != nullptr);
        if (!mcrv->is_mesh_degenerated())
            recover_curve(m,
                          msurface,
                          mcrv,
                          recover_map_inv,
                          edges_to_recover,
                          edges_not_recovered,
                          1);
    }

    // effectively recover the mesh curve
    for (auto & mcrv : msurface.curves()) {
        assert(mcrv != nullptr);
        if (!mcrv->is_mesh_degenerated()) {
            if (!recover_curve(m,
                               msurface,
                               mcrv,
                               recover_map_inv,
                               edges_to_recover,
                               edges_not_recovered,
                               2)) {
                return false;
            }
        }
    }

    Log::debug("Recovering {} mesh edges ({} not recovered)",
               edges_to_recover.size(),
               edges_not_recovered.size());

    if (edges_not_recovered.size() /*|| gf->meshStatistics.refineAllEdges*/) {
        // std::ostringstream sstream;
        // for (auto itr = edgesNotRecovered.begin(); itr != edgesNotRecovered.end(); ++itr)
        //     sstream << " " << itr->ge->tag();
        // if (gf->meshStatistics.refineAllEdges) {
        //     Msg::Info("8-| Splitting all edges and trying again");
        // }
        // else {
        //     Msg::Info(":-( There are %d intersections in the 1D mesh (curves%s)",
        //               edgesNotRecovered.size(),
        //               sstream.str().c_str());
        //     if (repairSelfIntersecting1dMesh)
        //         Msg::Info("8-| Splitting those edges and trying again");
        // }

        if (repair_self_intersecting_1d_mesh) {
            remesh_unrecovered_edges(recover_map_inv, edges_not_recovered);
            // gf->meshStatistics.refineAllEdges = false;
        }
        else {
            for (auto & edge : edges_not_recovered) {
                int p1 = edge.p1;
                int p2 = edge.p2;
                throw Exception("Edge not recovered: {} {} {}", p1, p2, edge.ge->id());
            }
        }

        if (recur_iter < 10) {
            return mesh_generator(msurface,
                                  recur_iter + 1,
                                  repair_self_intersecting_1d_mesh,
                                  only_initial_mesh,
                                  debug);
        }
        return false;
    }

    if (recur_iter > 0)
        Log::info(":-) All edges recovered after {} iteration{}",
                  recur_iter,
                  (recur_iter > 1) ? "s" : "");

    Log::debug("Boundary edges recovered for surface {}", msurface.id());

    // look for a triangle that has a negative node and recursively tag all
    // exterior triangles
    for (auto & tri : m.triangles) {
        assert(tri != nullptr);
        tri->g = nullptr;
    }
    for (auto & tri : m.triangles) {
        BDS_Point * n[4];
        if (tri->get_nodes(n)) {
            assert(n[0] != nullptr);
            assert(n[1] != nullptr);
            assert(n[2] != nullptr);
            if (n[0]->iD < 0 || n[1]->iD < 0 || n[2]->iD < 0) {
                recur_tag(tri, &CLASS_EXTERIOR);
                break;
            }
        }
    }

    // now find an edge that has belongs to one of the exterior triangles
    for (auto & e : m.edges) {
        assert(e != nullptr);
        if (e->g && e->num_faces() == 2) {
            assert(e->faces(0) != nullptr);
            assert(e->faces(1) != nullptr);
            if (e->faces(0)->g == &CLASS_EXTERIOR) {
                recur_tag(e->faces(1), &CLASS_F);
                break;
            }
            else if (e->faces(1)->g == &CLASS_EXTERIOR) {
                recur_tag(e->faces(0), &CLASS_F);
                break;
            }
        }
    }
    for (auto & tri : m.triangles) {
        assert(tri != nullptr);
        if (tri->g == &CLASS_EXTERIOR)
            tri->g = nullptr;
    }

    for (auto & e : m.edges) {
        assert(e != nullptr);
        if (e->g && e->num_faces() == 2) {
            BDS_Point * oface[2];
            e->opposite_of(oface);
            assert(oface[0] != nullptr);
            assert(oface[1] != nullptr);
            if (oface[0]->iD < 0) {
                recur_tag(e->faces(1), &CLASS_F);
                break;
            }
            else if (oface[1]->iD < 0) {
                recur_tag(e->faces(0), &CLASS_F);
                break;
            }
        }
    }

    for (auto & e : msurface.embedded_curves()) {
        assert(e != nullptr);
        if (!e->is_mesh_degenerated())
            recover_curve(m,
                          msurface,
                          e,
                          recover_map_inv,
                          edges_to_recover,
                          edges_not_recovered,
                          2);
    }

    // compute characteristic lengths at vertices
    if (!only_initial_mesh) {
        Log::debug("Computing mesh size field at mesh nodes {}", edges_to_recover.size());
        for (auto & pp : m.points) {
            auto itv = recover_map.find(pp);
            if (itv != recover_map.end()) {
                assert(pp != nullptr);

                // auto * here = itv->second;
                // GEntity * ge = here->onWhat();
                // if (ge->dim() == 0) {
                //     pp->lcBGM() = BGM_MeshSize(ge, 0, 0, here->x(), here->y(), here->z());
                // }
                // else if (ge->dim() == 1) {
                //     double u;
                //     here->getParameter(0, u);
                //     pp->lcBGM() = BGM_MeshSize(ge, u, 0, here->x(), here->y(), here->z());
                // }
                // else
                //     pp->lcBGM() = MAX_LC;
                pp->lcBGM() = MAX_LC;
                pp->lc() = pp->lcBGM();
            }
        }
    }

    // delete useless stuff
    for (auto & tri : m.triangles) {
        assert(tri != nullptr);
        if (!tri->g)
            m.del_face(tri);
    }
    m.cleanup();

    for (auto & e : m.edges) {
        assert(e != nullptr);
        if (e->num_faces() == 0)
            m.del_edge(e);
        else {
            if (!e->g)
                e->g = &CLASS_F;
            assert(e->p1 != nullptr);
            if (!e->p1->g || e->p1->g->classif_degree > e->g->classif_degree)
                e->p1->g = e->g;
            assert(e->p2 != nullptr);
            if (!e->p2->g || e->p2->g->classif_degree > e->g->classif_degree)
                e->p2->g = e->g;
        }
    }
    m.cleanup();
    m.del_point(m.find_point(-1));
    m.del_point(m.find_point(-2));
    m.del_point(m.find_point(-3));
    m.del_point(m.find_point(-4));

    if (true) {
        for (auto & t : m.triangles) {
            assert(t != nullptr);
            if (!t->deleted) {
                BDS_Point * n[4];
                if (t->get_nodes(n)) {
                    auto * v1 = recover_map[n[0]];
                    auto * v2 = recover_map[n[1]];
                    auto * v3 = recover_map[n[2]];
                    if (!n[3]) {
                        if (v1 != v2 && v1 != v3 && v2 != v3)
                            msurface.add_triangle({ v1, v2, v3 });
                    }
                    else {
                        auto * v4 = recover_map[n[3]];
                        msurface.add_quadrangle({ v1, v2, v3, v4 });
                    }
                }
            }
        }
    }

    {
        int nb_swap;
        Log::debug("Delaunizing the initial mesh");
        delaunayize_bds(msurface, m, nb_swap);
    }

    // only delete the mesh data stored in the base GFace class
    msurface.delete_mesh();

    Log::debug("Starting to add internal nodes");
    // start mesh generation

    // fill the small gmsh structures
    convert_from_bds(m, msurface, recover_map);

    bool infty = false;
    // the delaunay algo is based directly on internal gmsh structures BDS mesh is
    // passed in order not to recompute local coordinates of vertices
    if (!only_initial_mesh) {
        bowyer_watson_frontal(msurface);
    }

    // remove unused vertices, generated e.g. during background mesh
    delete_unused_vertices(msurface);

    return true;
}

} // namespace

//

SchemeFrontalDelaunay::SchemeFrontalDelaunay() : Scheme("frontal-delaunay") {}

void
SchemeFrontalDelaunay::select_curve_scheme(MeshCurve & curve)
{
    // TODO: Eventually we want to give the meshing algoritm freedom to mesh edges
    // for us. For things where we would have mesh size in vertices, etc. we should use
    // `sizemap` scheme.
    if (curve.scheme().name() == "auto") {
        curve.set_scheme("equal").set("intervals", 1);
    }
}

void
SchemeFrontalDelaunay::mesh_surface(MeshSurface & surface)
{
    Log::info("Meshing surface {}: scheme='frontal-delaunay'", surface.id());

    mesh_generator(surface, 0, true, false, false);

    // fill in the vertices from curves (no need to go down to mesh vertices, they are included
    // in the curves)
    std::map<int, bool> v_added;
    for (auto & crv : surface.curves()) {
        for (auto & v : crv->bounding_vertices()) {
            auto nm = v->num();
            if (v_added.find(nm) == v_added.end()) {
                surface.add_vertex(v);
                v_added[nm] = true;
            }
        }
        for (auto & v : crv->curve_vertices()) {
            auto nm = v->num();
            if (v_added.find(nm) == v_added.end()) {
                surface.add_vertex(v);
                v_added[nm] = true;
            }
        }
    }
}

} // namespace krado
