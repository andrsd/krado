// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// original file: meshGFaceBDS.cpp

#include "krado/background_mesh_tools.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/geom_surface.h"
#include "krado/bds.h"
#include "krado/quality_measures.h"
#include "krado/scheme/frontal_delaunay_insertion.h"
#include "krado/scheme/frontal_delaunay_optimize.h"
#include "krado/types.h"
#include "krado/uv_param.h"
#include <map>

using namespace krado;

static void
get_degenerated_vertices(BDS_Mesh & m,
                         std::map<BDS_Point *, MeshVertexAbstract *, PointLessThan> * recover_map,
                         std::set<MeshVertexAbstract * /*, MVertexPtrLessThan*/> & degenerated,
                         std::vector<BDS_Edge *> & degenerated_edges)
{
    degenerated.clear();
    for (auto & e : m.edges) {
        assert(e != nullptr);
        if (!e->deleted && e->num_faces() == 1) {
            assert(recover_map != nullptr);
            auto itp1 = recover_map->find(e->p1);
            auto itp2 = recover_map->find(e->p2);
            if (itp1 != recover_map->end() && itp2 != recover_map->end() &&
                itp1->second == itp2->second) {
                degenerated.insert(itp1->second);
                degenerated_edges.push_back(e);
            }
        }
    }
}

static double
compute_edge_linear_length(BDS_Point * p1, BDS_Point * p2)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    return utils::distance(Point(p1->x, p1->y, p1->z), Point(p2->x, p2->y, p2->z));
}

static inline double
compute_edge_linear_length(BDS_Point * p1, BDS_Point * p2, const GeomSurface & f)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);

    Point GP;
    try {
        UVParam uv(0.5 * (p1->u + p2->u), 0.5 * (p1->v + p2->v));
        GP = f.point(uv);
    }
    catch (...) {
        return compute_edge_linear_length(p1, p2);
    }

    const double l1 = utils::distance(Point(p1->x, p1->y, p1->z), GP);
    const double l2 = utils::distance(Point(p2->x, p2->y, p2->z), GP);
    return l1 + l2;
}

static double
compute_edge_linear_length(const BDS_Edge * e, const GeomSurface & f)
{
    // FIXME !!!
    // return f.geomType() == GEntity::Plane ? e->length() : compute_edge_linear_length(e->p1,
    // e->p2, f);
    throw Exception("compute_edge_linear_length is not implemented yet");
}

static double
new_get_lc(BDS_Point * point, const MeshSurface & msurface)
{
    assert(point != nullptr);
    if (extend_1d_mesh_in_2d_surfaces(msurface))
        return std::min(point->lc(), point->lcBGM());
    else
        return point->lcBGM();
}

static double
correct_lc(BDS_Point * p1, BDS_Point * p2, const MeshSurface & msurface)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);

    auto l1 = new_get_lc(p1, msurface);
    auto l2 = new_get_lc(p2, msurface);
    auto l = .5 * (l1 + l2);

    const double coord = 0.5;
    UVParam uv { coord * p1->u + (1 - coord) * p2->u, coord * p1->v + (1 - coord) * p2->v };

    auto & f = msurface.geom_surface();
    auto gpp = f.point(uv);
    // FIXME: double lmid = BGM_MeshSize(f, U, V, gpp.x, gpp.y, gpp.z);
    // FIXME: l = std::min(l, lmid);

    // FIXME: if (CTX::instance()->mesh.lcFromCurvature) {
    // FIXME:     double l3 = l;
    // FIXME:     double const lcmin = std::min(std::min(l1, l2), l3);
    // FIXME:     l1 = std::min(lcmin * 1.2, l1);
    // FIXME:     l2 = std::min(lcmin * 1.2, l2);
    // FIXME:     l3 = std::min(lcmin * 1.2, l3);
    // FIXME:     l = (l1 + l2 + l3) / 3.0;
    // FIXME: }
    return l;
}

static double
new_get_lc(const BDS_Edge * edge, const MeshSurface & msurface)
{
    assert(edge != nullptr);
    return compute_edge_linear_length(edge, msurface.geom_surface()) /
           correct_lc(edge->p1, edge->p2, msurface);
}

// SWAP TESTS i.e. tell if swap should be done

static bool
edge_swap_test_angle(BDS_Edge * e, double min_cos)
{
    auto * f1 = e->faces(0);
    auto * f2 = e->faces(1);
    BDS_Point * n1[4];
    BDS_Point * n2[4];
    assert(f1 != nullptr);
    assert(f2 != nullptr);
    if (!f1->get_nodes(n1) || !f2->get_nodes(n2))
        return false;
    auto norm1 = normal_triangle(n1[0], n1[1], n1[2]);
    auto norm2 = normal_triangle(n2[0], n2[1], n2[2]);

    return dot_product(norm1, norm2) > min_cos;
}

static int
edge_swap_test(const GeomSurface & gf, BDS_Edge * e)
{
    BDS_Point * op[2];

    // FIXME: this is a global setting
    double allow_swap_edge_angle = 10.;
    const double thresh = std::cos(allow_swap_edge_angle * M_PI / 180.);

    assert(e != nullptr);
    e->opposite_of(op);

    auto qa1 = quality::gamma<ElementType::TRI3>({ e->p1, e->p2, op[0] });
    auto qa2 = quality::gamma<ElementType::TRI3>({ e->p1, e->p2, op[1] });
    auto qb1 = quality::gamma<ElementType::TRI3>({ e->p1, op[0], op[1] });
    auto qb2 = quality::gamma<ElementType::TRI3>({ e->p2, op[0], op[1] });
    auto qa = std::min(qa1, qa2);
    auto qb = std::min(qb1, qb2);

    //  if(qb > 15*qa) return 1;

    if (!edge_swap_test_angle(e, thresh))
        return -1;

    if (qb > qa)
        return 1;
    else
        return -1;
}

static bool
neighboring_modified(BDS_Point * p)
{
    assert(p != nullptr);
    if (p->config_modified)
        return true;
    for (auto & e : p->edges) {
        assert(e != nullptr);
        auto * o = e->other_vertex(p);
        assert(o != nullptr);
        if (o->config_modified)
            return true;
    }
    return false;
}

static void
swap_edge_pass(const GeomSurface & gf,
               BDS_Mesh & m,
               int & nb_swap,
               bool finalize = false,
               double orientation = 1.0)
{
    BDS_SwapEdgeTest * qual;
    if (finalize)
        qual = new BDS_SwapEdgeTestNormals(gf, orientation);
    else
        qual = new BDS_SwapEdgeTestQuality(true, true);

    using size_type = std::vector<BDS_Edge *>::size_type;
    size_type orig_size = m.edges.size();
    for (size_type index = 0; index < 2 * orig_size && index < m.edges.size(); ++index) {
        if (neighboring_modified(m.edges.at(index)->p1) ||
            neighboring_modified(m.edges.at(index)->p2)) {
            if (!m.edges.at(index)->deleted && m.edges.at(index)->num_faces() == 2) {
                const int result = finalize ? 1 : edge_swap_test(gf, m.edges.at(index));
                if (result >= 0) {
                    if (m.swap_edge(m.edges.at(index), *qual))
                        ++nb_swap;
                }
            }
        }
    }
    m.cleanup();
    delete qual;
}

static bool
edge_swap_test_delaunay_aniso(BDS_Edge * e, const GeomSurface & gf, std::set<SwapQuad> & configs)
{
    BDS_Point * op[2];

    assert(e != nullptr);
    assert(e->p1 != nullptr);
    assert(e->p2 != nullptr);
    if (!e->p1->config_modified && !e->p2->config_modified)
        return false;

    if (e->num_faces() != 2)
        return false;

    e->opposite_of(op);

    SwapQuad sq(e->p1->iD, e->p2->iD, op[0]->iD, op[1]->iD);
    if (configs.find(sq) != configs.end())
        return false;
    configs.insert(sq);

    UVParam edge_center(0.5 * (e->p1->u + e->p2->u), 0.5 * (e->p1->v + e->p2->v));

    UVParam p1(e->p1->u, e->p1->v);
    UVParam p2(e->p2->u, e->p2->v);
    UVParam p3(op[0]->u, op[0]->v);
    UVParam p4(op[1]->u, op[1]->v);
    auto metric = build_metric(gf, edge_center);
    if (!in_circum_circle_aniso(gf, p1, p2, p3, p4, metric))
        return false;
    return true;
}

namespace krado {

void
delaunayize_bds(MeshSurface & surf, BDS_Mesh & m, int & nb_swap)
{
    nb_swap = 0;
    std::set<SwapQuad> configs;
    while (true) {
        std::size_t nsw = 0;
        for (auto & edge : m.edges) {
            assert(edge != nullptr);
            if (!edge->deleted) {
                if (edge_swap_test_delaunay_aniso(edge, surf.geom_surface(), configs)) {
                    if (m.swap_edge(edge, BDS_SwapEdgeTestQuality(false)))
                        ++nsw;
                }
            }
        }
        nb_swap += nsw;
        if (!nsw)
            return;
    }
}

} // namespace krado

static bool
edges_sort(std::pair<double, BDS_Edge *> a, std::pair<double, BDS_Edge *> b)
{
    // @note: don't compare pointers: it leads to non-deterministic behavior
    if (std::abs(a.first - b.first) < 1e-10) {
        if (a.second->p1->iD == b.second->p1->iD) {
            assert(a.second->p2 != nullptr);
            assert(b.second->p2 != nullptr);
            return (a.second->p2->iD < b.second->p2->iD);
        }
        else {
            assert(a.second->p1 != nullptr);
            assert(b.second->p1 != nullptr);
            return (a.second->p1->iD < b.second->p1->iD);
        }
    }
    else
        return (a.first < b.first);
}

static std::tuple<UVParam, bool>
middle_point(const GeomSurface & gsurface, BDS_Edge * e)
{
    assert(e != nullptr);
    assert(e->p1 != nullptr);
    Point pt1(e->p1->x, e->p1->y, e->p1->z);
    UVParam uv1(e->p1->u, e->p1->v);
    assert(e->p2 != nullptr);
    Point pt2(e->p2->x, e->p2->y, e->p2->z);
    UVParam uv2(e->p2->u, e->p2->v);

    int iter = 0;
    while (true) {
        auto mid = 0.5 * (uv1 + uv2);
        auto gpp = gsurface.point(mid);
        double l1 = utils::distance(gpp, pt1);
        double l2 = utils::distance(gpp, pt2);
        // 1 ------ p -- 2
        if (l1 > 1.2 * l2)
            uv2 = mid;
        else if (l2 > 1.2 * l1)
            uv1 = mid;
        else
            return { mid, true };

        if (iter++ > 4) {
            auto u = 0.5 * (e->p1->u + e->p2->u);
            auto v = 0.5 * (e->p1->v + e->p2->v);
            return { { u, v }, false };
        }
    }
}

// create a valid initial mesh when degeneracies are present

static void
get_degeneracy(BDS_Mesh & m, std::vector<BDS_Point *> & deg)
{
    deg.clear();
    for (auto & p : m.points) {
        assert(p != nullptr);
        if (p->degenerated)
            deg.push_back(p);
    }
}

static void
set_degeneracy(std::vector<BDS_Point *> & deg, short d)
{
    for (auto & p : deg) {
        assert(p != nullptr);
        p->degenerated = d;
    }
}

static void
split_all_edges_connected_to_singularity(const GeomSurface & gsurface, BDS_Mesh & m)
{
    std::vector<BDS_Edge *> degenerated;
    for (auto & e : m.edges) {
        assert(e != nullptr);
        assert(e->p1 != nullptr);
        assert(e->p2 != nullptr);
        if (!e->deleted && ((!e->p1->degenerated && e->p2->degenerated) ||
                            (e->p1->degenerated && !e->p2->degenerated)))
            degenerated.push_back(e);
    }
    for (auto & e : degenerated) {
        assert(e != nullptr);
        if (!e->deleted && e->num_faces() == 2) {
            assert(e->p1 != nullptr);
            assert(e->p2 != nullptr);
            UVParam UV(0.5 * (e->p1->u + e->p2->u), 0.5 * (e->p1->v + e->p2->v));
            auto gpp = gsurface.point(UV);
            BDS_Point * mid = m.add_point(++m.MAXPOINTNUMBER, gpp.x, gpp.y, gpp.z);
            mid->u = UV.u;
            mid->v = UV.v;
            mid->lc() = 0.5 * (e->p1->lc() + e->p2->lc());
            // abort if one of the splits leads to an invalid mesh in the param plane
            if (!m.split_edge(e, mid, true)) {
                m.del_point(mid);
                return;
            }
        }
    }
}

static void
split_edge_pass(const MeshSurface & msurface,
                BDS_Mesh & m,
                double maxe,
                int & nb_split,
                std::vector<UVParam> * true_boundary)
{
    auto & gf = msurface.geom_surface();
    std::vector<std::pair<double, BDS_Edge *>> edges;

    auto [u_lo, u_hi] = gf.param_range(0);
    auto [v_lo, v_hi] = gf.param_range(1);
    UVParam out { u_hi + 1.21982512, v_hi + 1.8635436432 };

    for (auto & p : m.points) {
        assert(p != nullptr);
        if (!p->periodic_counterpart && (p->g && p->g->classif_degree == 2)) {
            for (size_t i = 0; i < p->edges.size(); ++i) {
                assert(p->edges[i] != nullptr);
                auto * p1 = p->edges[i]->p1 == p ? p->edges[i]->p2 : p->edges[i]->p1;
                for (size_t j = 0; j < i; ++j) {
                    auto * p2 = p->edges[j]->p1 == p ? p->edges[j]->p2 : p->edges[j]->p1;
                    if (!p1->degenerated && !p2->degenerated && p1->periodic_counterpart &&
                        p1->periodic_counterpart == p2) {
                        edges.push_back(std::make_pair(-10.0, p->edges[i]));
                        edges.push_back(std::make_pair(-10.0, p->edges[j]));
                    }
                }
            }
        }
    }

    for (auto & e : m.edges) {
        assert(e != nullptr);
        if (!e->deleted && e->num_faces() == 2 && e->g && e->g->classif_degree == 2) {
            auto lone = new_get_lc(e, msurface);
            if (lone > maxe)
                edges.push_back(std::make_pair(-lone, e));
        }
    }

    std::sort(edges.begin(), edges.end(), edges_sort);

    std::vector<BDS_Point *> mids(edges.size());

    // FIXME: DA: bool faceDiscrete = gf->geomType() == GEntity::DiscreteSurface;
    bool face_discrete = false;

    for (std::size_t i = 0; i < edges.size(); ++i) {
        BDS_Edge * e = edges[i].second;
        BDS_Point * mid = nullptr;
        assert(e != nullptr);
        if (!e->deleted && (neighboring_modified(e->p1) || neighboring_modified(e->p2))) {
            assert(e->p1 != nullptr);
            assert(e->p2 != nullptr);

            UVParam uv1(e->p1->u, e->p1->v);
            UVParam uv2(e->p2->u, e->p2->v);
            if (e->p1->degenerated == 1)
                uv1.u = uv2.u;
            if (e->p2->degenerated == 1)
                uv2.u = uv1.u;
            if (e->p1->degenerated == 2)
                uv1.v = uv2.v;
            if (e->p2->degenerated == 2)
                uv2.v = uv1.v;
            UVParam mid_uv = 0.5 * (uv1 + uv2);
            if (face_discrete) {
                auto [mid_uv, b] = middle_point(gf, e);
                if (!b)
                    continue;
            }

            auto gpp = gf.point(mid_uv);
            bool inside = true;
            if (true_boundary) {
                int N;
                if (!point_inside_parametric_domain(*true_boundary, mid_uv, out, N))
                    inside = false;
            }
            if (inside) {
                mid = m.add_point(++m.MAXPOINTNUMBER, gpp.x, gpp.y, gpp.z);
                mid->u = mid_uv.u;
                mid->v = mid_uv.v;
                mid->lc() = 0.5 * (e->p1->lc() + e->p2->lc());
                // FIXME: DA: mid->lcBGM() = BGM_MeshSize(gf, U, V, mid->X, mid->Y, mid->Z);
            }
        }
        mids[i] = mid;
    }

    for (std::size_t i = 0; i < edges.size(); ++i) {
        auto * e = edges[i].second;
        assert(e != nullptr);
        if (!e->deleted) {
            auto * mid = mids[i];
            if (mid) {
                if (!m.split_edge(e, mid))
                    m.del_point(mid);
                else
                    nb_split++;
            }
        }
    }
}

double
get_max_lc_when_collapsing_edge(const MeshSurface & msurface,
                                BDS_Mesh & m,
                                BDS_Edge * e,
                                BDS_Point * p)
{
    assert(e != nullptr);
    assert(p != nullptr);

    auto * o = e->other_vertex(p);

    double max_lc = 0.0;
    std::vector<BDS_Edge *> edges(p->edges);
    for (auto & e : edges) {
        assert(e != nullptr);
        BDS_Point *new_p1 = nullptr, *new_p2 = nullptr;
        if (e->p1 == p) {
            new_p1 = o;
            new_p2 = e->p2;
        }
        else if (e->p2 == p) {
            new_p1 = e->p1;
            new_p2 = o;
        }
        if (!new_p1 || !new_p2)
            break; // error
        BDS_Edge collapsed_edge(new_p1, new_p2);
        max_lc = std::max(max_lc, new_get_lc(&collapsed_edge, msurface));
        new_p1->del(&collapsed_edge);
        new_p2->del(&collapsed_edge);
    }

    return max_lc;
}

void
collapse_edge_pass(const MeshSurface & msurface,
                   BDS_Mesh & m,
                   double mine,
                   int max_np,
                   int & nb_collaps)
{
    std::vector<std::pair<double, BDS_Edge *>> edges;
    for (auto & e : m.edges) {
        assert(e != nullptr);
        if (!e->deleted && e->num_faces() == 2 && e->g && e->g->classif_degree == 2) {
            double lone = new_get_lc(e, msurface);
            if (lone < mine)
                edges.push_back(std::make_pair(lone, e));
        }
    }

    std::sort(edges.begin(), edges.end(), edges_sort);

    for (std::size_t i = 0; i < edges.size(); ++i) {
        BDS_Edge * e = edges[i].second;
        assert(e != nullptr);
        if (!e->deleted && (neighboring_modified(e->p1) || neighboring_modified(e->p2))) {
            double lone1 = 0.;
            bool collapse_p1_allowed = false;
            assert(e->p1 != nullptr);
            if (e->p1->iD > max_np) {
                lone1 = get_max_lc_when_collapsing_edge(msurface, m, e, e->p1);
                collapse_p1_allowed = std::abs(lone1 - 1.0) < std::abs(edges[i].first - 1.0);
            }

            double lone2 = 0.;
            bool collapse_p2_allowed = false;
            assert(e->p2 != nullptr);
            if (e->p2->iD > max_np) {
                lone2 = get_max_lc_when_collapsing_edge(msurface, m, e, e->p2);
                collapse_p2_allowed = std::abs(lone2 - 1.0) < std::abs(edges[i].first - 1.0);
            }

            BDS_Point * p = nullptr;
            if (collapse_p1_allowed && collapse_p2_allowed) {
                if (std::abs(lone1 - lone2) < 1e-12)
                    p = e->p1->iD < e->p2->iD ? e->p1 : e->p2;
                else
                    p = std::abs(lone1 - 1.0) < std::abs(lone2 - 1.0) ? e->p1 : e->p2;
            }
            else if (collapse_p1_allowed && !collapse_p2_allowed)
                p = e->p1;
            else if (collapse_p2_allowed && !collapse_p1_allowed)
                p = e->p2;

            if (p && m.collapse_edge_parametric(e, p))
                nb_collaps++;
        }
    }
}

void
smooth_vertex_pass(const GeomSurface & gf, BDS_Mesh & m, int & nb_smooth, bool q, double threshold)
{
    for (int i = 0; i < 1; ++i) {
        for (auto & p : m.points) {
            if (neighboring_modified(p)) {
                if (m.smooth_point_centroid(p, gf, threshold))
                    nb_smooth++;
            }
        }
    }
}
