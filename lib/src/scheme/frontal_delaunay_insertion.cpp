// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// Original file: meshGFaceDelaunayInsertion.cpp

#include "krado/mesh_element.h"
#include "krado/mesh_surface.h"
#include "krado/scheme/frontal_delaunay_insertion.h"
#include "krado/scheme/frontal_delaunay_optimize.h"
#include "krado/mesh_curve.h"
#include "krado/geom_surface.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/numerics.h"
#include "krado/types.h"
#include "krado/uv_param.h"
#include "krado/predicates.h"
#include "krado/quality_measures.h"
#include "krado/intersect_curve_surface.h"
#include "krado/consts.h"
#include "krado/background_mesh_tools.h"
#include "robust_predicates/robust_predicates.h"
#include <Eigen/Eigen>
#include <limits>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>

namespace krado {

static const double LIMIT = 0.5 * std::sqrt(2.0) * 1;
int MTri3::radius_norm = 2;

std::set<MeshVertex *>
get_degenerated_vertices(const MeshSurface & msurface)
{
    std::set<MeshVertex *> vtxs;
    for (auto & mcrv : msurface.curves()) {
        assert(mcrv != nullptr);
        auto & bnd_vtx = mcrv->bounding_vertices();
        if (bnd_vtx[0] && bnd_vtx[0] == bnd_vtx[1]) {
            // FIXME
            // if (e->geom_type() == GEntity::Unknown)
            //     vtxs.insert(bnd_vtx[0]);
        }
    }

    return vtxs;
}

static inline bool
intersection_segments_2(double * p1, double * p2, double * q1, double * q2)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(q1 != nullptr);
    assert(q2 != nullptr);
    auto a = robust_predicates::orient2d(p1, p2, q1);
    auto b = robust_predicates::orient2d(p1, p2, q2);
    if (a * b > 0)
        return 0;
    a = robust_predicates::orient2d(q1, q2, p1);
    b = robust_predicates::orient2d(q1, q2, p2);
    if (a * b > 0)
        return 0;
    return 1;
}

static inline bool
intersection_segments_2(const UVParam & p1,
                        const UVParam & p2,
                        const UVParam & q1,
                        const UVParam & q2)
{
    auto a = orient2d(p1, p2, q1);
    auto b = orient2d(p1, p2, q2);
    if (a * b > 0)
        return 0;
    a = orient2d(q1, q2, p1);
    b = orient2d(q1, q2, p2);
    if (a * b > 0)
        return 0;
    return 1;
}

static bool
is_active(MTri3 * t, double limit, int & active)
{
    assert(t != nullptr);
    if (t->is_deleted())
        return false;
    for (active = 0; active < 3; active++) {
        auto * neigh = t->neighbor(active);
        if (!neigh || (neigh->radius() < limit && neigh->radius() > 0))
            return true;
    }
    return false;
}

static bool
is_active(MTri3 * t, double limit, int & i, std::set<MeshElement, MEdgeLessThan> * front)
{
    assert(t != nullptr);
    if (t->is_deleted())
        return false;
    for (i = 0; i < 3; i++) {
        auto * neigh = t->neighbor(i);
        if (!neigh || (neigh->radius() < limit && neigh->radius() > 0)) {
            int ip1 = i - 1 < 0 ? 2 : i - 1;
            int ip2 = i;
            auto me = MeshElement::Line2({ t->tri().vertex(ip1), t->tri().vertex(ip2) });
            assert(front != nullptr);
            if (front->find(me) != front->end())
                return true;
        }
    }
    return false;
}

static void
update_active_edges(MTri3 * t, double limit, std::set<MeshElement, MEdgeLessThan> & front)
{
    assert(t != nullptr);
    if (t->is_deleted())
        return;
    for (int active = 0; active < 3; active++) {
        auto * neigh = t->neighbor(active);
        if (!neigh || (neigh->radius() < limit && neigh->radius() > 0)) {
            int ip1 = active - 1 < 0 ? 2 : active - 1;
            int ip2 = active;
            auto me = MeshElement::Line2({ t->tri().vertex(ip1), t->tri().vertex(ip2) });
            front.insert(me);
        }
    }
}

static std::tuple<UVParam, double>
circum_center_metric(UVParam pa, UVParam pb, UVParam pc, std::tuple<double, double, double> metric)
{
    // d = (u2-u1) M (u2-u1) = u2 M u2 + u1 M u1 - 2 u2 M u1
    Eigen::Matrix2d sys;
    Eigen::Vector2d rhs;

    const auto [a, b, d] = metric;

    auto [pa_u, pa_v] = pa;
    auto [pb_u, pb_v] = pb;
    auto [pc_u, pc_v] = pc;

    sys(0, 0) = 2. * a * (pa_u - pb_u) + 2. * b * (pa_v - pb_v);
    sys(0, 1) = 2. * d * (pa_v - pb_v) + 2. * b * (pa_u - pb_u);
    sys(1, 0) = 2. * a * (pa_u - pc_u) + 2. * b * (pa_v - pc_v);
    sys(1, 1) = 2. * d * (pa_v - pc_v) + 2. * b * (pa_u - pc_u);

    rhs(0) = a * (pa_u * pa_u - pb_u * pb_u) + d * (pa_v * pa_v - pb_v * pb_v) +
             2. * b * (pa_u * pa_v - pb_u * pb_v);
    rhs(1) = a * (pa_u * pa_u - pc_u * pc_u) + d * (pa_v * pa_v - pc_v * pc_v) +
             2. * b * (pa_u * pa_v - pc_u * pc_v);

    auto x = sys2x2(sys, rhs);
    auto radius = (x[0] - pa_u) * (x[0] - pa_u) * a + (x[1] - pa_v) * (x[1] - pa_v) * d +
                  2. * (x[0] - pa_u) * (x[1] - pa_v) * b;
    return { { x[0], x[1] }, radius };
}

static std::tuple<UVParam, double>
circum_center_metric(const MeshElement & base,
                     std::tuple<double, double, double> metric,
                     BidimMeshData & data)
{
    // d = (u2-u1) M (u2-u1) = u2 M u2 + u1 M u1 - 2 u2 M u1
    auto index0 = data.index(base.vertex(0));
    auto index1 = data.index(base.vertex(1));
    auto index2 = data.index(base.vertex(2));
    auto pa = data.uv[index0];
    auto pb = data.uv[index1];
    auto pc = data.uv[index2];
    return circum_center_metric(pa, pb, pc, metric);
}

std::tuple<double, double, double>
build_metric(const GeomSurface & geom_surface, UVParam uv)
{
    auto [ddx, ddy] = geom_surface.d1(uv);
    return { dot_product(ddx, ddx), dot_product(ddy, ddx), dot_product(ddy, ddy) };
}

static double
compute_tolerance(const double radius)
{
    if (radius <= 1e3)
        return 1e-12;
    else if (radius <= 1e5)
        return 1e-11;
    else
        return 1e-9;
}

bool
in_circum_circle_aniso(const GeomSurface & surface,
                       UVParam p1,
                       UVParam p2,
                       UVParam p3,
                       UVParam uv,
                       std::tuple<double, double, double> metric)
{
    auto [x, radius2] = circum_center_metric(p1, p2, p3, metric);
    const auto [a, b, d] = metric;
    const double d0 = (x.u - uv.u);
    const double d1 = (x.v - uv.v);
    const double d3 = d0 * d0 * a + d1 * d1 * d + 2.0 * d0 * d1 * b;
    const double tolerance = compute_tolerance(radius2);
    return d3 < radius2 - tolerance;
}

bool
in_circum_circle_aniso(const GeomSurface & surface,
                       const MeshElement & base,
                       UVParam uv,
                       std::tuple<double, double, double> * metricb,
                       BidimMeshData & data)
{
    UVParam x;
    double radius2;
    std::tuple<double, double, double> metric;
    if (metricb == nullptr) {
        UVParam pa(0., 0.);
        for (int i = 0; i < 3; ++i) {
            int index = data.index(base.vertex(i));
            pa += data.uv[index];
        }
        pa /= 3.;

        metric = build_metric(surface, pa);
    }
    else {
        metric = *metricb;
    }
    std::tie(x, radius2) = circum_center_metric(base, metric, data);

    const auto [a, b, d] = metric;

    const double d0 = (x.u - uv.u);
    const double d1 = (x.v - uv.v);
    const double d3 = d0 * d0 * a + d1 * d1 * d + 2.0 * d0 * d1 * b;
    return d3 < radius2;
}

static Point
fourth_point(Point p1, Point p2, Point p3)
{
    auto c = circum_center_xyz(p1, p2, p3);
    auto vx = p2 - p1;
    auto vy = p3 - p1;
    auto vz = cross_product(vx, vy).normalized();
    double R = (p1 - c).magnitude();
    return c + R * vz;
}

MTri3::MTri3(const MeshElement & t, double lc, BidimMeshData * data, MeshSurface * surface) :
    deleted_(false),
    base_(t),
    neigh_ { nullptr, nullptr, nullptr }
{
    assert(this->base_.type() == ElementType::TRI3);

    auto pa = this->base_.vertex(0)->point();
    auto pb = this->base_.vertex(1)->point();
    auto pc = this->base_.vertex(2)->point();

    if (radius_norm == 3) {
        this->circum_radius_ = 1. / quality::gamma(this->base_);
    }
    else if (radius_norm == 2) {
        auto center = circum_center_xyz(pa, pb, pc);
        auto delta = this->base_.vertex(0)->point() - center;
        this->circum_radius_ = delta.magnitude() / lc;
    }
    else {
        assert(data != nullptr);
        auto index0 = data->index(this->base_.vertex(0));
        auto index1 = data->index(this->base_.vertex(1));
        auto index2 = data->index(this->base_.vertex(2));
        auto p1 = data->uv[index0];
        auto p2 = data->uv[index1];
        auto p3 = data->uv[index2];

        UVParam midpoint(0., 0.);
        for (int i = 0; i < 3; ++i) {
            auto index = data->index(this->base_.vertex(i));
            midpoint += data->uv[index];
        }
        midpoint /= 3.;

        // FIXME: vvv
        // double quadAngle =
        //     backgroundMesh::current()
        //         ? backgroundMesh::current()->getAngle(midpoint[0], midpoint[1], 0)
        //         : 0.0;
        double quad_angle = 0.;

        double x0 = p1.u * std::cos(quad_angle) + p1.v * std::sin(quad_angle);
        double y0 = -p1.u * std::sin(quad_angle) + p1.v * std::cos(quad_angle);
        double x1 = p2.u * std::cos(quad_angle) + p2.v * std::sin(quad_angle);
        double y1 = -p2.u * std::sin(quad_angle) + p2.v * std::cos(quad_angle);
        double x2 = p3.u * std::cos(quad_angle) + p3.v * std::sin(quad_angle);
        double y2 = -p3.u * std::sin(quad_angle) + p3.v * std::cos(quad_angle);
        double xmax = std::max(std::max(x0, x1), x2);
        double ymax = std::max(std::max(y0, y1), y2);
        double xmin = std::min(std::min(x0, x1), x2);
        double ymin = std::min(std::min(y0, y1), y2);

        auto metric = build_metric(surface->geom_surface(), midpoint);
        double RATIO = std::pow(std::get<0>(metric) * std::get<2>(metric) -
                                    std::get<1>(metric) * std::get<1>(metric),
                                -0.25);

        this->circum_radius_ = std::max(xmax - xmin, ymax - ymin) / (RATIO * lc);
    }
}

int
MTri3::in_circum_circle(const Point & p) const
{
    auto pa = this->base_.vertex(0)->point();
    auto pb = this->base_.vertex(1)->point();
    auto pc = this->base_.vertex(2)->point();
    auto fourth = fourth_point(pa, pb, pc);
    double result = insphere(pa, pb, pc, fourth, p) * orient3d(pa, pb, pc, fourth);
    return (result > 0) ? 1 : 0;
}

int
in_circum_circle(const MeshElement & base,
                 const UVParam & p,
                 const UVParam & param,
                 BidimMeshData & data)
{
    int index0 = data.index(base.vertex(0));
    int index1 = data.index(base.vertex(1));
    int index2 = data.index(base.vertex(2));
    auto pa = data.uv[index0];
    auto pb = data.uv[index1];
    auto pc = data.uv[index2];
    double result = incircle(pa, pb, pc, param) * orient2d(pa, pb, pc);
    return (result > 0) ? 1 : 0;
}

template <class Iterator>
static void
connect_tris(Iterator beg, Iterator end, std::vector<EdgeXFace> & conn)
{
    conn.clear();

    while (beg != end) {
        if (!(*beg)->is_deleted()) {
            for (int j = 0; j < 3; ++j)
                conn.push_back(EdgeXFace(*beg, j));
        }
        ++beg;
    }

    if (conn.empty())
        return;

    std::sort(conn.begin(), conn.end());

    for (std::size_t i = 0; i < conn.size() - 1; ++i) {
        auto & f1 = conn[i];
        auto & f2 = conn[i + 1];

        if (f1 == f2 && f1.t1 != f2.t1) {
            f1.t1->set_neighbor(f1.i1, f2.t1);
            f2.t1->set_neighbor(f2.i1, f1.t1);
            ++i;
        }
    }
}

void
connect_triangles(std::list<MTri3 *> & l)
{
    std::vector<EdgeXFace> conn;
    connect_tris(l.begin(), l.end(), conn);
}

void
connect_triangles(std::vector<MTri3 *> & l)
{
    std::vector<EdgeXFace> conn;
    connect_tris(l.begin(), l.end(), conn);
}

void
connect_triangles(std::set<MTri3 *, MTri3::ComparePtr> & l)
{
    std::vector<EdgeXFace> conn;
    connect_tris(l.begin(), l.end(), conn);
}

static int
in_circum_circle_xy(const MeshElement & t, MeshVertexAbstract * v)
{
    auto v1 = t.vertex(0)->point();
    auto v2 = t.vertex(1)->point();
    auto v3 = t.vertex(2)->point();
    double p1[2] = { v1.x, v1.y };
    double p2[2] = { v2.x, v2.y };
    double p3[2] = { v3.x, v3.y };
    assert(v != nullptr);
    auto pt = v->point();
    double pp[2] = { pt.x, pt.y };
    double result =
        robust_predicates::incircle(p1, p2, p3, pp) * robust_predicates::orient2d(p1, p2, p3);
    return (result > 0) ? 1 : 0;
}

static void
recur_find_cavity(std::vector<EdgeXFace> & shell,
                  std::vector<MTri3 *> & cavity,
                  MeshVertexAbstract * v,
                  MTri3 * t)
{
    assert(t != nullptr);
    t->set_deleted(true);
    // the cavity that has to be removed because it violates delaunay criterion
    cavity.push_back(t);

    for (int i = 0; i < 3; ++i) {
        auto * neigh = t->neighbor(i);
        if (!neigh)
            shell.push_back(EdgeXFace(t, i));
        else if (!neigh->is_deleted()) {
            int circ = in_circum_circle_xy(neigh->tri(), v);
            if (circ)
                recur_find_cavity(shell, cavity, v, neigh);
            else
                shell.push_back(EdgeXFace(t, i));
        }
    }
}

static void
recur_find_cavity_aniso(const MeshSurface & gf,
                        std::list<EdgeXFace> & shell,
                        std::list<MTri3 *> & cavity,
                        std::tuple<double, double, double> metric,
                        UVParam param,
                        MTri3 * t,
                        BidimMeshData & data)
{
    assert(t != nullptr);
    t->set_deleted(true);
    // the cavity that has to be removed because it violates delaunay criterion
    cavity.push_back(t);

    for (int i = 0; i < 3; ++i) {
        auto neigh = t->neighbor(i);
        EdgeXFace exf(t, i);
        // take care of untouchable internal edges
        auto it = data.internal_edges.find(MeshElement::Line2({ exf.v[0], exf.v[1] }));
        if (!neigh || it != data.internal_edges.end())
            shell.push_back(exf);
        else if (!neigh->is_deleted()) {
            int circ =
                in_circum_circle_aniso(gf.geom_surface(), neigh->tri(), param, &metric, data);
            if (circ)
                recur_find_cavity_aniso(gf, shell, cavity, metric, param, neigh, data);
            else
                shell.push_back(exf);
        }
    }
}

static std::tuple<UVParam, bool>
circ_uv(const MeshElement & t, BidimMeshData & data)
{
    auto index0 = data.index(t.vertex(0));
    auto index1 = data.index(t.vertex(1));
    auto index2 = data.index(t.vertex(2));
    auto u1 = data.uv[index0];
    auto u2 = data.uv[index1];
    auto u3 = data.uv[index2];
    return { circum_center_xy(u1, u2, u3), true };
}

static std::tuple<UVParam, bool>
inv_map_uv(const MeshElement & t, UVParam p, BidimMeshData & data, double tol)
{
    auto index0 = data.index(t.vertex(0));
    auto index1 = data.index(t.vertex(1));
    auto index2 = data.index(t.vertex(2));

    auto [u0, v0] = data.uv[index0];
    auto [u1, v1] = data.uv[index1];
    auto [u2, v2] = data.uv[index2];

    Eigen::Matrix2d mat;
    mat(0, 0) = u1 - u0;
    mat(0, 1) = u2 - u0;
    mat(1, 0) = v1 - v0;
    mat(1, 1) = v2 - v0;
    Eigen::Vector2d b;
    b[0] = p.u - u0;
    b[1] = p.v - v0;
    auto uv = sys2x2(mat, b);

    auto in = uv[0] >= -tol && uv[1] >= -tol && uv[0] <= 1. + tol && uv[1] <= 1. + tol &&
              1. - uv[0] - uv[1] > -tol;
    return { { uv[0], uv[1] }, in };
}

inline double
get_surf_uv(const MeshElement & t, BidimMeshData & data)
{
    int index0 = data.index(t.vertex(0));
    int index1 = data.index(t.vertex(1));
    int index2 = data.index(t.vertex(2));

    auto [u1, v1] = data.uv[index0];
    auto [u2, v2] = data.uv[index1];
    auto [u3, v3] = data.uv[index2];

    const double vv1[2] = { u2 - u1, v2 - v1 };
    const double vv2[2] = { u3 - u1, v3 - v1 };

    return 0.5 * (vv1[0] * vv2[1] - vv1[1] * vv2[0]);
}

static int
insert_vertex_b(std::list<EdgeXFace> & shell,
                std::list<MTri3 *> & cavity,
                bool force,
                MeshSurface & surface,
                MeshVertexAbstract * v,
                const UVParam & param,
                MTri3 * t,
                std::set<MTri3 *, MTri3::ComparePtr> & all_tets,
                std::set<MTri3 *, MTri3::ComparePtr> * active_tets,
                BidimMeshData & data,
                std::tuple<double, double, double> metric,
                MTri3 ** one_new_triangle,
                bool verify_star_shapeness = true)
{
    if (cavity.size() == 1)
        return -1;

    if (shell.size() != cavity.size() + 2)
        return -2;

    double EPS = verify_star_shapeness ? 1.e-12 : 1.e12;

    // check that volume is conserved
    double new_volume = 0.0;
    double new_min_quality = 2.0;

    double old_volume =
        std::accumulate(begin(cavity),
                        end(cavity),
                        0.0,
                        [&](double volume, MTri3 * const triangle) {
                            assert(triangle != nullptr);
                            return volume + std::abs(get_surf_uv(triangle->tri(), data));
                        });

    std::vector<MTri3 *> new_tris(shell.size());

    std::vector<MTri3 *> new_cavity;

    int k = 0;

    bool one_point_is_too_close = false;

    for (auto & it : shell) {
        MeshVertexAbstract *v0, *v1;
        if (it.ori > 0) {
            v0 = it.v[0];
            v1 = it.v[1];
        }
        else {
            v0 = it.v[1];
            v1 = it.v[0];
        }
        assert(v0 != nullptr);
        assert(v1 != nullptr);
        assert(v != nullptr);
        auto t = MeshElement::Tri3({ v0, v1, v });
        int index0 = data.index(t.vertex(0));
        int index1 = data.index(t.vertex(1));
        int index2 = data.index(t.vertex(2));
        constexpr double ONE_THIRD = 1. / 3.;
        double lc =
            ONE_THIRD * (data.v_sizes[index0] + data.v_sizes[index1] + data.v_sizes[index2]);
        double lc_bgm = ONE_THIRD * (data.v_sizes_bgm[index0] + data.v_sizes_bgm[index1] +
                                     data.v_sizes_bgm[index2]);
        double LL = std::min(lc, lc_bgm);

        auto ll = extend_1d_mesh_in_2d_surfaces(surface) ? LL : lc_bgm;
        MTri3 * t4 = new MTri3(t, ll, &data, &surface);

        if (one_new_triangle) {
            force = true;
            *one_new_triangle = t4;
        }

        double d1 = utils::distance(v0->point(), v->point());
        double d2 = utils::distance(v1->point(), v->point());
        double d3 = utils::distance(v0->point(), v1->point());
        double d4 = 1.e22;
        // avoid angles that are too obtuse
        double cosv = ((d1 * d1 + d2 * d2 - d3 * d3) / (2. * d1 * d2));

        if (v0->geom_shape().dim() != 2 && v1->geom_shape().dim() != 2) {
            auto v0v1 = v1->point() - v0->point();
            auto v0v = v->point() - v0->point();
            auto pv = cross_product(v0v1, v0v);
            d4 = pv.magnitude() / d3;
        }

        if ((d1 < LL * .5 || d2 < LL * .5 || d4 < LL * .4 || cosv < -.9999) && !force) {
            one_point_is_too_close = true;
        }

        new_tris[k++] = t4;
        // all new triangles are pushed front in order to be able to destroy them if
        // the cavity is not star shaped around the new vertex.
        new_cavity.push_back(t4);

        MTri3 * otherSide = it.t1->neighbor(it.i1);
        if (otherSide)
            new_cavity.push_back(otherSide);

        double ss = std::abs(get_surf_uv(t4->tri(), data));
        if (ss < 1.e-25)
            ss = 1.e22;

        new_volume += ss;
        new_min_quality = std::min(new_min_quality, quality::gamma(t4->tri()));
    }

    std::vector<EdgeXFace> conn;

    // for adding a point we require that the area remains the same after addition
    // of the point, and that the point is not too close to an edge
    if (std::abs(old_volume - new_volume) < EPS * old_volume && !one_point_is_too_close) {
        connect_tris(new_cavity.begin(), new_cavity.end(), conn);
        // 30 % of the time is spent here!
        all_tets.insert(new_tris.begin(), new_tris.end());
        if (active_tets) {
            for (auto i = new_cavity.begin(); i != new_cavity.end(); ++i) {
                int active_edge;
                if (is_active(*i, LIMIT, active_edge) && (*i)->radius() > LIMIT) {
                    if ((*active_tets).find(*i) == (*active_tets).end())
                        (*active_tets).insert(*i);
                }
            }
        }
        return 1;
    }
    else {
        // the cavity is NOT star shaped
        std::for_each(begin(cavity), end(cavity), [](MTri3 * triangle) {
            assert(triangle != nullptr);
            triangle->set_deleted(false);
        });
        for (std::size_t i = 0; i < shell.size(); ++i) {
            delete new_tris[i];
        }

        if (std::abs(old_volume - new_volume) > EPS * old_volume)
            return -3;
        if (one_point_is_too_close)
            return -4;
        return -5;
    }
}

static bool
inv_map_xy(const MeshElement & t, MeshVertexAbstract * v)
{
    assert(v != nullptr);

    auto v0 = t.vertex(0)->point();
    auto v1 = t.vertex(1)->point();
    auto v2 = t.vertex(2)->point();
    double tol = 1.e-6;
    Eigen::Matrix2d mat;
    mat(0, 0) = v1.x - v0.x;
    mat(0, 1) = v2.x - v0.x;
    mat(1, 0) = v1.y - v0.y;
    mat(1, 1) = v2.y - v0.y;
    Eigen::Vector2d b;
    auto pt = v->point();
    b(0) = pt.x - v0.x;
    b(1) = pt.y - v0.y;
    auto uv = sys2x2(mat, b);

    if (uv[0] >= -tol && uv[1] >= -tol && uv[0] <= 1. + tol && uv[1] <= 1. + tol &&
        1. - uv[0] - uv[1] > -tol) {
        return true;
    }
    return false;
}

static MTri3 *
search_for_triangle(MTri3 * t, MeshVertexAbstract * v, int maxx, int & iter)
{
    assert(t != nullptr);
    assert(v != nullptr);

    bool inside = inv_map_xy(t->tri(), v);
    UVParam q1 { v->point().x, v->point().y };
    if (inside)
        return t;
    while (1) {
        auto bc = t->tri().barycenter();
        UVParam q2 { bc.x, bc.y };
        int i;
        for (i = 0; i < 3; ++i) {
            int i1 = i == 0 ? 2 : i - 1;
            int i2 = i;
            auto * v1 = t->tri().vertex(i1);
            auto * v2 = t->tri().vertex(i2);
            assert(v1 != nullptr);
            assert(v2 != nullptr);
            UVParam p1 { v1->point().x, v1->point().y };
            UVParam p2 { v2->point().x, v2->point().y };
            if (intersection_segments_2(p1, p2, q1, q2))
                break;
        }
        if (i >= 3)
            break;
        t = t->neighbor(i);
        if (!t)
            break;
        bool inside = inv_map_xy(t->tri(), v);
        if (inside)
            return t;
        if (iter++ > .5 * maxx)
            break;
    }
    return nullptr;
}

static MTri3 *
search_for_triangle(MTri3 * t,
                    UVParam pt,
                    BidimMeshData & data,
                    std::set<MTri3 *, MTri3::ComparePtr> & all_tris,
                    UVParam & uv,
                    bool force = false)
{
    assert(t != nullptr);

    bool inside;
    std::tie(uv, inside) = inv_map_uv(t->tri(), pt, data, 1.e-8);

    if (inside)
        return t;
    UVParam q1 { pt.u, pt.v };
    int iter = 0;
    while (1) {
        int index0 = data.index(t->tri().vertex(0));
        int index1 = data.index(t->tri().vertex(1));
        int index2 = data.index(t->tri().vertex(2));
        UVParam q2 = (1. / 3.) * (data.uv[index0] + data.uv[index1] + data.uv[index2]);
        int i;
        for (i = 0; i < 3; i++) {
            int i1 = data.index(t->tri().vertex(i == 0 ? 2 : i - 1));
            int i2 = data.index(t->tri().vertex(i));
            auto p1 = data.uv[i1];
            auto p2 = data.uv[i2];
            if (intersection_segments_2(p1, p2, q1, q2))
                break;
        }
        if (i >= 3)
            throw Exception("impossible");
        t = t->neighbor(i);
        if (!t)
            break;
        bool inside;
        std::tie(uv, inside) = inv_map_uv(t->tri(), pt, data, 1.e-8);
        if (inside)
            return t;
        if (iter++ > (int) all_tris.size())
            break;
    }

    if (!force)
        return nullptr; // @note removing this leads to horrible performance

    for (auto & tri : all_tris) {
        assert(tri != nullptr);
        if (!tri->is_deleted()) {
            std::tie(uv, inside) = inv_map_uv(tri->tri(), pt, data, 1.e-8);
            if (inside)
                return tri;
        }
    }
    return nullptr;
}

static bool
insert_a_point(MeshSurface & gf,
               std::set<MTri3 *, MTri3::ComparePtr>::iterator it,
               UVParam center,
               std::tuple<double, double, double> metric,
               BidimMeshData & data,
               std::set<MTri3 *, MTri3::ComparePtr> & all_tris,
               std::set<MTri3 *, MTri3::ComparePtr> * active_tris = nullptr,
               MTri3 * worst = nullptr,
               MTri3 ** one_new_triangle = nullptr,
               bool test_star_shapeness = false)
{
    if (worst) {
        it = all_tris.find(worst);
        if (worst != *it)
            throw Exception("Could not insert point");
    }
    else
        worst = *it;

    MTri3 * ptin = nullptr;
    std::list<EdgeXFace> shell;
    std::list<MTri3 *> cavity;
    UVParam uv;

    // if the point is able to break the bad triangle "worst"
    if (in_circum_circle_aniso(gf.geom_surface(), worst->tri(), center, &metric, data)) {
        recur_find_cavity_aniso(gf, shell, cavity, metric, center, worst, data);
        for (auto & tc : cavity) {
            auto [uv, in] = inv_map_uv(tc->tri(), center, data, 1.e-8);
            if (in) {
                ptin = tc;
                break;
            }
        }
    }
    else {
        ptin =
            search_for_triangle(worst, center, data, all_tris, uv, one_new_triangle ? true : false);
        if (ptin)
            recur_find_cavity_aniso(gf, shell, cavity, metric, center, ptin, data);
    }

    if (ptin) {
        // we use here local coordinates as real coordinates x,y and z will be
        // computed hereafter
        auto sv = new MeshSurfaceVertex(gf.geom_surface(), center);

        double lc1, lc;
        int index0 = data.index(ptin->tri().vertex(0));
        int index1 = data.index(ptin->tri().vertex(1));
        int index2 = data.index(ptin->tri().vertex(2));
        lc1 = (1. - uv.u - uv.v) * data.v_sizes[index0] + uv.u * data.v_sizes[index1] +
              uv.v * data.v_sizes[index2];
        // FIXME: DA: if (CTX::instance()->mesh.algo2d == ALGO_2D_BAMG)
        // FIXME: DA:     lc = 1.;
        // FIXME: DA: else
        // FIXME: DA:     lc = BGM_MeshSize(gf, center[0], center[1], p.x(), p.y(), p.z());
        lc = .5; /// ^^^^

        data.add_vertex(sv, center, lc1, lc);

        int result = -9;
        // if (p.succeeded()) {
        result = insert_vertex_b(shell,
                                 cavity,
                                 false,
                                 gf,
                                 sv,
                                 center,
                                 ptin,
                                 all_tris,
                                 active_tris,
                                 data,
                                 metric,
                                 one_new_triangle,
                                 test_star_shapeness);
        // }
        if (result != 1) {
            // if (result == -1)
            //     Msg::Debug("Point %g %g cannot be inserted because cavity if of size 1",
            //                center[0],
            //                center[1]);
            // if (result == -2)
            //     Msg::Debug("Point %g %g cannot be inserted because euler formula is "
            //                "not fulfilled",
            //                center[0],
            //                center[1]);
            // if (result == -3)
            //     Msg::Debug("Point %g %g cannot be inserted because cavity is not star shaped",
            //                center[0],
            //                center[1]);
            // if (result == -4)
            //     Msg::Debug("Point %g %g cannot be inserted because it is too close to "
            //                "another point)",
            //                center[0],
            //                center[1]);
            // if (result == -5)
            //     Msg::Debug("Point %g %g cannot be inserted because it is out of the "
            //                "parametric domain)",
            //                center[0],
            //                center[1]);

            all_tris.erase(it);
            worst->force_radius(-1);
            all_tris.insert(worst);
            delete sv;
            for (auto & tc : cavity) {
                assert(tc != nullptr);
                tc->set_deleted(false);
            }
            return false;
        }
        else {
            gf.add_vertex(sv);
            return true;
        }
    }
    else {
        for (auto & tc : cavity) {
            assert(tc != nullptr);
            tc->set_deleted(false);
        }
        all_tris.erase(it);
        worst->force_radius(0);
        all_tris.insert(worst);
        return false;
    }
}

void
bowyer_watson(MeshSurface & msurface,
              int max_pnt,
              std::map<MeshVertexAbstract *, MeshVertexAbstract *> * equivalence,
              std::map<MeshVertexAbstract *, UVParam> * parametricCoordinates)
{
    std::set<MTri3 *, MTri3::ComparePtr> all_tris;
    BidimMeshData data(equivalence, parametricCoordinates);

    if (!build_mesh_generation_data_structures(msurface, all_tris, data)) {
        // Msg::Error("Invalid meshing data structure");
        return;
    }

    if (all_tris.empty())
        throw Exception("No triangles in initial mesh");

    int iter = 0;
    while (1) {
        MTri3 * worst = *all_tris.begin();
        assert(worst != nullptr);
        if (worst->is_deleted()) {
            delete worst;
            all_tris.erase(all_tris.begin());
        }
        else {
            if (iter++ % 5000 == 0) {
                // Msg::Debug("%7d points created -- Worst tri radius is %8.3f",
                //            DATA.vSizes.size(),
                //            worst->getRadius());
            }
            // VERIFY STOP !!!
            if (worst->radius() < 0.5 * std::sqrt(2.0) || (int) data.v_sizes.size() > max_pnt)
                break;

            double r2;
            auto [center, b] = circ_uv(worst->tri(), data);
            const auto & base = worst->tri();
            int index0 = data.index(base.vertex(0));
            int index1 = data.index(base.vertex(1));
            int index2 = data.index(base.vertex(2));
            UVParam pa = (1. / 3.) * (data.uv[index0] + data.uv[index1] + data.uv[index2]);
            auto metric = build_metric(msurface.geom_surface(), pa);
            std::tie(center, r2) = circum_center_metric(worst->tri(), metric, data);
            insert_a_point(msurface, all_tris.begin(), center, metric, data, all_tris);
        }
    }
    transfer_data_structure(msurface, all_tris, data);
}

// Let's try a frontal delaunay approach now that the delaunay mesher is stable.
// We use the approach of Rebay (JCP 1993) that we extend to anisotropic
// metrics. The point isertion is done on the Voronoi Edges.

static double
length_infnite_norm(const double p[2], const double q[2], const double quad_angle)
{
    auto xp = p[0] * std::cos(quad_angle) + p[1] * std::sin(quad_angle);
    auto yp = -p[0] * std::sin(quad_angle) + p[1] * std::cos(quad_angle);
    auto xq = q[0] * std::cos(quad_angle) + q[1] * std::sin(quad_angle);
    auto yq = -q[0] * std::sin(quad_angle) + q[1] * std::cos(quad_angle);
    auto xmax = std::max(xp, xq);
    auto xmin = std::min(xp, xq);
    auto ymax = std::max(yp, yq);
    auto ymin = std::min(yp, yq);
    return std::max(xmax - xmin, ymax - ymin);
}

static std::array<double, 2>
circum_center_infinite(MeshElement * base, double quad_angle, const BidimMeshData & data)
{
    assert(base != nullptr);

    const int index0 = data.index(base->vertex(0));
    const int index1 = data.index(base->vertex(1));
    const int index2 = data.index(base->vertex(2));
    auto pa = data.uv[index0];
    auto pb = data.uv[index1];
    auto pc = data.uv[index2];
    const double xa = pa.u * std::cos(quad_angle) - pa.v * std::sin(quad_angle);
    const double ya = pa.u * std::sin(quad_angle) + pa.v * std::cos(quad_angle);
    const double xb = pb.u * std::cos(quad_angle) - pb.v * std::sin(quad_angle);
    const double yb = pb.u * std::sin(quad_angle) + pb.v * std::cos(quad_angle);
    const double xc = pc.u * std::cos(quad_angle) - pc.v * std::sin(quad_angle);
    const double yc = pc.u * std::sin(quad_angle) + pc.v * std::cos(quad_angle);
    const double xmax = std::max(std::max(xa, xb), xc);
    const double ymax = std::max(std::max(ya, yb), yc);
    const double xmin = std::min(std::min(xa, xb), xc);
    const double ymin = std::min(std::min(ya, yb), yc);
    return { 0.5 * (xmax - xmin), 0.5 * (ymax - ymin) };
}

static double
length_metric(const UVParam & p, const UVParam & q, const std::tuple<double, double, double> metric)
{
    return std::sqrt((p.u - q.u) * std::get<0>(metric) * (p.u - q.u) +
                     2 * (p.u - q.u) * std::get<1>(metric) * (p.v - q.v) +
                     (p.v - q.v) * std::get<2>(metric) * (p.v - q.v));
}

/*
          /|
         / |
        /  |
       /   |
   lc /    |  r
     /     |
    /      |
   /       x
  /        |
 /         |  r/2
/          |
-----------+
     lc/2

     (3 r/2)^2 = lc^2 - lc^2/4
     -> lc^2 3/4 = 9r^2/4
     -> lc^2 = 3 r^2

     r^2 /4 + lc^2/4 = r^2
     -> lc^2 = 3 r^2

*/

static double
optimal_point_frontal(MeshSurface & msurface,
                      MTri3 * worst,
                      int active_edge,
                      BidimMeshData & data,
                      UVParam & new_point,
                      std::tuple<double, double, double> & metric)
{
    assert(worst != nullptr);

    UVParam center;
    double r2;
    auto base = worst->tri();
    bool ins;
    std::tie(center, ins) = circ_uv(base, data);
    int index0 = data.index(base.vertex(0));
    int index1 = data.index(base.vertex(1));
    int index2 = data.index(base.vertex(2));
    UVParam pa = (1. / 3.) * (data.uv[index0] + data.uv[index1] + data.uv[index2]);
    metric = build_metric(msurface.geom_surface(), pa);
    std::tie(center, r2) = circum_center_metric(worst->tri(), metric, data);
    // compute the middle point of the edge
    int ip1 = active_edge - 1 < 0 ? 2 : active_edge - 1;
    int ip2 = active_edge;

    index0 = data.index(base.vertex(ip1));
    index1 = data.index(base.vertex(ip2));
    UVParam P = data.uv[index0];
    UVParam Q = data.uv[index1];
    UVParam midpoint = 0.5 * (P + Q);

    // now we have the edge center and the center of the circumcircle, we try to
    // find a point that would produce a perfect triangle while connecting the 2
    // points of the active edge
    double dir[2] = { center.u - midpoint.u, center.v - midpoint.v };
    double norm = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1]);
    dir[0] /= norm;
    dir[1] /= norm;
    const double RATIO = std::sqrt(dir[0] * dir[0] * std::get<0>(metric) +
                                   2 * dir[1] * dir[0] * std::get<1>(metric) +
                                   dir[1] * dir[1] * std::get<2>(metric));

    const double rhoM1 = 0.5 * (data.v_sizes[index0] + data.v_sizes[index1]);
    const double rhoM2 = 0.5 * (data.v_sizes_bgm[index0] + data.v_sizes_bgm[index1]);
    const double rhoM = extend_1d_mesh_in_2d_surfaces(msurface) ? std::min(rhoM1, rhoM2) : rhoM2;
    const double rhoM_hat = rhoM;

    const double q = length_metric(center, midpoint, metric);
    const double d = rhoM_hat * std::sqrt(3.0) * 0.5;

    // d is corrected in a way that the mesh size is computed at point newPoint

    const double L = std::min(d, q);

    new_point.u = midpoint.u + L * dir[0] / RATIO;
    new_point.v = midpoint.v + L * dir[1] / RATIO;

    return L;
}

/*
            x
            |
            |
            | d =  3^{1/2}/2 h
            |
            |
      ------p------->   n
            h

   x point of the plane

   h being some kind of average between the size field
   and the edge length
*/

static bool
optimal_point_frontal_b(MeshSurface & msurface,
                        MTri3 * worst,
                        int active_edge,
                        BidimMeshData & data,
                        UVParam & new_point,
                        std::tuple<double, double, double> & metric)
{
    assert(worst != nullptr);

    // as a starting point, let us use the "fast algo"
    double d = optimal_point_frontal(msurface, worst, active_edge, data, new_point, metric);
    int ip1 = (active_edge + 2) % 3;
    int ip2 = active_edge;
    int ip3 = (active_edge + 1) % 3;
    auto v1 = worst->tri().vertex(ip1)->point();
    auto v2 = worst->tri().vertex(ip2)->point();
    auto v3 = worst->tri().vertex(ip3)->point();
    auto middle = 0.5 * (v1 + v2);
    auto v1v2 = v2 - v1;
    auto tmp = v3 - middle;
    auto n1 = cross_product(v1v2, tmp);
    if (n1.magnitude() < 1.e-12)
        return true;
    auto n2 = cross_product(n1, v1v2);
    n1.normalize();
    n2.normalize();
    // we look for a point that is
    // P = d * (n1 std::cos(t) + n2 std::sin(t)) that is on the surface
    // so we have to find t, starting with t = 0
    //  return true;

#if defined(HAVE_HXT)
    if (gf->geomType() == GEntity::DiscreteSurface) {
        discreteFace * ddf = dynamic_cast<discreteFace *>(gf);
        if (ddf) {
            GPoint gp = ddf->intersectionWithCircle(n1, n2, middle, d, newPoint);
            if (gp.succeeded())
                return true;
            return false;
        }
    }
#endif

    double uvt[3] = { new_point.u, new_point.v, 0.0 };
    CurveFunctorCircle cc(n2, n1, middle, d);
    SurfaceFunctorGFace ss(msurface.geom_surface());

    if (intersect_curve_surface(cc, ss, uvt, d * 1.e-8)) {
        new_point.u = uvt[0];
        new_point.v = uvt[1];
        return true;
    }

    return true;
}

void
bowyer_watson_frontal(MeshSurface & msurface,
                      std::map<MeshVertexAbstract *, MeshVertexAbstract *> * equivalence,
                      std::map<MeshVertexAbstract *, UVParam> * parametric_coordinates,
                      std::vector<UVParam> * true_boundary)
{
    std::set<MTri3 *, MTri3::ComparePtr> all_tris;
    std::set<MTri3 *, MTri3::ComparePtr> active_tris;
    BidimMeshData data(equivalence, parametric_coordinates);
    bool test_star_shapeness = true;
    auto degenerated = get_degenerated_vertices(msurface);

    if (!build_mesh_generation_data_structures(msurface, all_tris, data)) {
        // Msg::Error("Invalid meshing data structure");
        return;
    }

    int iter = 0, active_edge;
    // compute active triangle
    for (auto & tri : all_tris) {
        assert(tri != nullptr);
        if (is_active(tri, LIMIT, active_edge))
            active_tris.insert(tri);
        else if (tri->radius() < LIMIT)
            break;
    }

    auto gf = msurface.geom_surface();
    auto [u_lo, u_hi] = gf.param_range(0);
    auto [v_lo, v_hi] = gf.param_range(1);
    UVParam far { 2 * u_hi, 2 * v_hi };

    // insert points
    while (1) {
        if (active_tris.empty())
            break;
        MTri3 * worst = (*active_tris.begin());
        active_tris.erase(active_tris.begin());

        assert(worst != nullptr);
        if (!worst->is_deleted() && is_active(worst, LIMIT, active_edge) &&
            worst->radius() > LIMIT) {
            // if (ITER++ % 5000 == 0)
            //     Msg::Debug("%7d points created -- Worst tri radius is %8.3f",
            //                surface.all_vertices().size(),
            //                worst->getRadius());
            UVParam new_point;
            std::tuple<double, double, double> metric;
            if (optimal_point_frontal_b(msurface, worst, active_edge, data, new_point, metric)) {
                UVParam NP = new_point;
                int nnnn;
                if (!true_boundary || point_inside_parametric_domain(*true_boundary, NP, far, nnnn))
                    insert_a_point(msurface,
                                   all_tris.end(),
                                   new_point,
                                   metric,
                                   data,
                                   all_tris,
                                   &active_tris,
                                   worst,
                                   nullptr,
                                   test_star_shapeness);
            }
        }
    }

    transfer_data_structure(msurface, all_tris, data);
}

static MTri3 *
get_tri_to_break(MeshVertexAbstract * v, std::vector<MTri3 *> & t, int & iter)
{
    // last inserted is used as starting point we know it is not deleted
    std::size_t k = t.size() - 1;
    while (t[k]->is_deleted())
        k--;

    MTri3 * start = t[k];
    start = search_for_triangle(start, v, (int) t.size(), iter);
    if (start)
        return start;
    for (size_t i = 0; i < t.size(); ++i) {
        assert(t[i] != nullptr);
        if (!t[i]->is_deleted() && in_circum_circle_xy(t[i]->tri(), v))
            return t[i];
    }
    return nullptr;
}

static bool
tri_on_box(MeshElement * t, const std::array<MeshVertexAbstract *, 4> & box)
{
    assert(t != nullptr);
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 4; j++) {
            if (t->vertex(i) == box[j])
                return true;
        }
    }
    return false;
}

// vertices are supposed to be sitting in the XY plane !

void recover_edges(std::vector<MTri3 *> & t, std::vector<Element> & edges);

bool
swap_edge(MeshVertexAbstract * v1,
          MeshVertexAbstract * v2,
          MeshVertexAbstract * v3,
          MeshVertexAbstract * v4,
          MTri3 * t1,
          int i_local_edge)
{
    assert(t1 != nullptr);
    MTri3 * t2 = t1->neighbor(i_local_edge);
    if (t2 == nullptr)
        return false;

    auto t1b = MeshElement::Tri3({ v2, v3, v4 });
    auto t2b = MeshElement::Tri3({ v4, v3, v1 });
    double before = t1->tri().volume() + t2->tri().volume();
    double after = t1b.volume() + t2b.volume();
    // printf("swapping %d %d %d %d %g %g\n",
    //        v1->getNum(),v2->getNum(),v3->getNum(),v4->getNum(),BEFORE,AFTER);
    if (std::abs(before - after) / before > 1.e-8)
        return false;

    t1->set_tri(t1b);
    t2->set_tri(t2b);

    std::set<MTri3 *> cavity;
    cavity.insert(t1);
    cavity.insert(t2);
    for (int i = 0; i < 3; ++i) {
        if (t1->neighbor(i))
            cavity.insert(t1->neighbor(i));
        if (t2->neighbor(i))
            cavity.insert(t2->neighbor(i));
    }
    std::vector<EdgeXFace> conn;
    connect_tris(cavity.begin(), cavity.end(), conn);
    return true;
}

bool
diff_end(MeshVertexAbstract * v1,
         MeshVertexAbstract * v2,
         MeshVertexAbstract * p1,
         MeshVertexAbstract * p2)
{
    if (v1 == p1 || v2 == p1 || v1 == p2 || v2 == p2)
        return false;
    return true;
}

static bool
recover_edge_by_swaps(std::vector<MTri3 *> & t,
                      MeshVertexAbstract * mv1,
                      MeshVertexAbstract * mv2,
                      std::vector<MeshElement> & edges)
{
    assert(mv1 != nullptr);
    assert(mv2 != nullptr);

    UVParam pv1 { mv1->point().x, mv1->point().y };
    UVParam pv2 { mv2->point().x, mv2->point().y };
    UVParam xcc;
    for (std::size_t i = 0; i < t.size(); ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            auto * v1 = t[i]->tri().vertex((j + 2) % 3);
            auto * v2 = t[i]->tri().vertex(j);
            auto * v3 = t[i]->tri().vertex((j + 1) % 3);
            auto * o = t[i]->other_side(j);
            if (o) {
                assert(v1 != nullptr);
                assert(v2 != nullptr);
                assert(v3 != nullptr);

                UVParam p1 { v1->point().x, v1->point().y };
                UVParam p2 { v2->point().x, v2->point().y };
                UVParam p3 { v3->point().x, v3->point().y };
                UVParam po { o->point().x, o->point().y };
                if (diff_end(v1, v2, mv1, mv2)) {
                    if (intersection_segments(p1, p2, pv1, pv2, xcc)) {
                        if (!intersection_segments(po, p3, pv1, pv2, xcc) ||
                            (v3 == mv1 || o == mv1 || v3 == mv2 || o == mv2)) {
                            if (swap_edge(v1, v2, v3, o, t[i], j))
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// recover the edges by edge swapping in the triangulation.
// edges are not supposed to

void
recover_edges(std::vector<MTri3 *> & t, std::vector<MeshElement> & edges)
{
    MEdgeLessThan le;
    std::sort(edges.begin(), edges.end(), le);
    std::set<MeshElement, MEdgeLessThan> set_of_mesh_edges;
    for (size_t i = 0; i < t.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
            assert(t[i] != nullptr);
            auto edge = t[i]->tri().get_edge(j);
            set_of_mesh_edges.insert(edge);
        }
    }

    std::vector<MeshElement> edges_to_recover;
    for (std::size_t i = 0; i < edges.size(); ++i) {
        if (set_of_mesh_edges.find(edges[i]) == set_of_mesh_edges.end())
            edges_to_recover.push_back(edges[i]);
    }

    // Msg::Info("%d edges to recover among %d edges", edgesToRecover.size(), edges.size());
    for (std::size_t i = 0; i < edges_to_recover.size(); ++i) {
        auto * mstart = edges_to_recover[i].vertex(0);
        auto * mend = edges_to_recover[i].vertex(1);
        // Msg::Info("recovering edge %d %d", mstart->getNum(), mend->getNum());
        while (recover_edge_by_swaps(t, mstart, mend, edges))
            ;
    }
}

} // namespace krado
