// SPDX-FileCopyrightText: 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-2.0

#include "krado/bds.h"
#include "krado/consts.h"
#include "krado/vector.h"
#include "krado/numerics.h"
#include "krado/geom_surface.h"
#include "krado/predicates.h"
#include "krado/element.h"
#include "krado/log.h"
#include "krado/ref.h"
#include <stack>

namespace krado {

namespace {

Vector
vector_triangle(Ref<const BDS_Point> p1, Ref<const BDS_Point> p2, Ref<const BDS_Point> p3)
{
    auto a = p1->point - p2->point;
    auto b = p1->point - p3->point;
    return cross_product(a, b);
}

double
vector_triangle_parametric(Ref<const BDS_Point> p1,
                           Ref<const BDS_Point> p2,
                           Ref<const BDS_Point> p3)
{
    auto a = p1->uv - p2->uv;
    auto b = p1->uv - p3->uv;
    return a.u * b.v - a.v * b.u;
}

Vector
normal_triangle(Ref<const BDS_Point> p1, Ref<const BDS_Point> p2, Ref<const BDS_Point> p3)
{
    return vector_triangle(p1, p2, p3).normalized();
}

double
_cos_N(Ref<const BDS_Point> p1,
       Ref<const BDS_Point> p2,
       Ref<const BDS_Point> p3,
       const GeomSurface * gf)
{
    auto n = normal_triangle(p1, p2, p3);
#if 0
      // average surface normal at 3 triangle nodes; bad for surface with high
      // curvature on small area, e.g. U-shaped near a boundary
      SVector3 N1 = gf->normal(SPoint2(p1->u, p1->v));
      SVector3 N2 = gf->normal(SPoint2(p2->u, p2->v));
      SVector3 N3 = gf->normal(SPoint2(p3->u, p3->v));
      SVector3 N = N1 + N2 + N3;
      N.normalize();
#else // surface normal at triangle barycenter
    auto uv = 1. / 3. * (p1->uv + p2->uv + p3->uv);
    auto N = gf->normal(uv);
#endif
    return dot_product(N, n);
}

double
surface_triangle_param2(Ref<const BDS_Point> p1, Ref<const BDS_Point> p2, Ref<const BDS_Point> p3)
{
    // FIXME
    // THIS ASSUMES DEGENERATED EDGES ALONG AXIS U !!!
    // SEEMS TO BE THE CASE WITH OCC

    // if (!p1 || !p2 || !p3) {
    //     Log::error("Invalid point in parametric triangle surface computation");
    //     return 0;
    // }

    double c;
    if ((p1->degenerated ? 1 : 0) + (p2->degenerated ? 1 : 0) + (p3->degenerated ? 1 : 0) > 1)
        c = 0; // vector_triangle_parametric(p1, p2, p3, c);
    else if (p1->degenerated == 1) {
        auto du = std::abs(p3->u() - p2->u());
        c = 2 * std::abs(0.5 * (p3->v() + p2->v()) - p1->v()) * du;
    }
    else if (p2->degenerated == 1) {
        auto du = std::abs(p3->u() - p1->u());
        c = 2 * std::abs(0.5 * (p3->v() + p1->v()) - p2->v()) * du;
    }
    else if (p3->degenerated == 1) {
        auto du = std::abs(p2->u() - p1->u());
        c = 2 * std::abs(0.5 * (p2->v() + p1->v()) - p3->v()) * du;
    }
    else if (p1->degenerated == 2) {
        auto dv = std::abs(p3->v() - p2->v());
        c = 2 * std::abs(0.5 * (p3->u() + p2->u()) - p1->u()) * dv;
    }
    else if (p2->degenerated == 2) {
        auto dv = std::abs(p3->v() - p1->v());
        c = 2 * std::abs(0.5 * (p3->u() + p1->u()) - p2->u()) * dv;
    }
    else if (p3->degenerated == 2) {
        auto dv = std::abs(p2->v() - p1->v());
        c = 2 * std::abs(0.5 * (p2->u() + p1->u()) - p3->u()) * dv;
    }
    else
        c = vector_triangle_parametric(p1, p2, p3);
    return (0.5 * c);
}

bool
intersect_edges_2d(UVParam p1, UVParam p2, UVParam q1, UVParam p4)
{
    std::array<std::array<double, 2>, 2> mat;
    std::array<double, 2> rhs;
    mat[0][0] = (p2.u - p1.u);
    mat[0][1] = -(p4.u - q1.u);
    mat[1][0] = (p2.v - p1.v);
    mat[1][1] = -(p4.v - q1.v);
    rhs[0] = q1.u - p1.u;
    rhs[1] = q1.v - p1.v;
    auto res = sys2x2(mat, rhs);
    if (not res.has_value())
        return false;
    auto x = res.value();
    if (x[0] >= 0.0 && x[0] <= 1.0 && x[1] >= 0.0 && x[1] <= 1.0)
        return true;
    return false;
}

std::tuple<UVParam, double>
centroid_uv(const std::vector<UVParam> & kernel, const std::vector<double> & lcs)
{
    double u = 0.;
    double v = 0.;
    double l = 0.;
    for (std::size_t i = 0; i < kernel.size(); ++i) {
        u += kernel[i].u;
        v += kernel[i].v;
        l += lcs[i];
    }
    u /= kernel.size();
    v /= kernel.size();
    l /= kernel.size();
    return { UVParam(u, v), l };
}

std::tuple<UVParam, double>
centroid_uv(const BDS_Point & p,
            const GeomSurface & gf,
            const std::vector<UVParam> & kernel,
            const std::vector<double> & lcs)
{
    double u = 0.;
    double v = 0.;
    double l = 0.;
    double fact_sum = 0;
    for (std::size_t i = 0; i < kernel.size(); ++i) {
        auto gp = gf.point(kernel[i]);
        auto delta_uv = p.uv - kernel[i];
        auto denom = dot_product(delta_uv, delta_uv);
        if (denom) {
            auto delta = p.point - gp;
            auto fact = std::sqrt(dot_product(delta, delta) / denom);
            fact_sum += fact;
            u += kernel[i].u * fact;
            v += kernel[i].v * fact;
            l += lcs[i] * fact;
        }
    }
    if (fact_sum) {
        u /= fact_sum;
        v /= fact_sum;
        l /= fact_sum;
    }
    return { UVParam(u, v), l };
}

/// Compute intersection of 2 edges
std::array<double, 2>
intersection(UVParam p1, UVParam p2, UVParam q1, UVParam q2)
{
    std::array<std::array<double, 2>, 2> A;
    A[0][0] = p2.u - p1.u;
    A[0][1] = q1.u - q2.u;
    A[1][0] = p2.v - p1.v;
    A[1][1] = q1.v - q2.v;
    std::array<double, 2> b = { q1.u - p1.u, q1.v - p1.v };
    auto res = sys2x2(A, b);
    return res.value();
}

} // namespace

BDS_GeomEntity::BDS_GeomEntity(i32 tag, i32 degree) : tag(tag), degree(degree) {}

bool
BDS_GeomEntity::operator<(const BDS_GeomEntity & other) const
{
    if (this->degree < other.degree)
        return true;
    if (this->degree > other.degree)
        return false;
    if (this->tag < other.tag)
        return true;
    return false;
}

bool
BDS_GeomEntity::operator==(const BDS_GeomEntity & other) const
{
    return this->degree == other.degree && this->tag == other.tag;
}

// BDS_Point

BDS_Point::BDS_Point(i32 id, Point pt) :
    lc(MAX_LC),
    point(pt),
    config_modified(true),
    degenerated(0),
    id(id)
{
}

BDS_Point::BDS_Point(i32 id, Point pt, UVParam uv, BDS_GeomEntity ge) :
    lc(MAX_LC),
    point(pt),
    uv(uv),
    config_modified(true),
    degenerated(0),
    id(id),
    ge(ge)
{
}

double
BDS_Point::u() const
{
    return this->uv.u;
}

double
BDS_Point::v() const
{
    return this->uv.v;
}

void
BDS_Point::del(BDSEdgeHandle eh)
{
    if (this->edges.empty())
        return;
    this->edges.erase(std::remove(this->edges.begin(), this->edges.end(), eh), this->edges.end());
}

// BDS_Edge

BDS_Edge::BDS_Edge(BDSPointHandle a, BDSPointHandle b, Optional<BDS_GeomEntity> ge) :
    active(true),
    p1(a < b ? a : b),
    p2(a < b ? b : a),
    ge(ge)
{
}

//

BDS_Face::BDS_Face(BDSEdgeHandle A, BDSEdgeHandle B, BDSEdgeHandle C, Optional<BDS_GeomEntity> ge) :
    active(true),
    e1(A),
    e2(B),
    e3(C),
    ge(ge)
{
}

//

EdgeToRecover::EdgeToRecover(BDSPointHandle p1, BDSPointHandle p2)
{
    if (p1 < p2) {
        this->p1_ = p1;
        this->p2_ = p2;
    }
    else {
        this->p2_ = p1;
        this->p1_ = p2;
    }
}

bool
EdgeToRecover::operator<(const EdgeToRecover & other) const
{
    if (this->p1_ < other.p1_)
        return true;
    if (this->p1_ > other.p1_)
        return false;
    if (this->p2_ < other.p2_)
        return true;
    return false;
}

// BDS_Mesh

BDS_Mesh::BDS_Mesh() = default;

const BDS_Point &
BDS_Mesh::get_point(BDSPointHandle pt) const
{
    return this->points_[pt.id];
}

BDS_Point &
BDS_Mesh::get_point(BDSPointHandle pt)
{
    return this->points_[pt.id];
}

BDSPointHandle
BDS_Mesh::add_point(int num, Point pt)
{
    this->points_.emplace_back(num, pt);
    BDSPointHandle pti(static_cast<u32>(this->points_.size() - 1));
    this->points_by_num_.emplace(num, pti);
    return pti;
}

BDSPointHandle
BDS_Mesh::add_point(int num, UVParam uv, const GeomSurface & geom_surface, BDS_GeomEntity ge)
{
    auto gp = geom_surface.point(uv);
    this->points_.emplace_back(num, gp, uv, ge);
    BDSPointHandle pti(static_cast<u32>(this->points_.size() - 1));
    this->points_by_num_.emplace(num, pti);
    return pti;
}

const BDS_Edge &
BDS_Mesh::get_edge(BDSEdgeHandle h) const
{
    return this->edges_[h.id];
}

BDS_Edge &
BDS_Mesh::get_edge(BDSEdgeHandle h)
{
    return this->edges_[h.id];
}

const BDS_Face &
BDS_Mesh::get_face(BDSFaceHandle h) const
{
    return this->triangles_[h.id];
}

BDS_Face &
BDS_Mesh::get_face(BDSFaceHandle h)
{
    return this->triangles_[h.id];
}

Optional<std::array<BDSPointHandle, 3>>
BDS_Mesh::get_nodes(BDSFaceHandle fh) const
{
    const auto & face = get_face(fh);
    std::array<Optional<BDSPointHandle>, 3> n = { common_vertex(face.e1, face.e3),
                                                  common_vertex(face.e1, face.e2),
                                                  common_vertex(face.e2, face.e3) };
    if (n[0] && n[1] && n[2])
        return std::array<BDSPointHandle, 3> { n[0].value(), n[1].value(), n[2].value() };
    Log::error("Invalid points in face");
    return std::nullopt;
}

Optional<BDSPointHandle>
BDS_Mesh::common_vertex(BDSEdgeHandle eh1, BDSEdgeHandle eh2) const
{
    const auto & edge1 = get_edge(eh1);
    const auto & edge2 = get_edge(eh2);

    if (edge1.p1 == edge2.p1 || edge1.p1 == edge2.p2)
        return edge1.p1;
    if (edge1.p2 == edge2.p1 || edge1.p2 == edge2.p2)
        return edge1.p2;
    Log::error("Edge {}-{} has no common node with edge {}-{}",
               edge1.p1.id,
               edge1.p2.id,
               edge2.p1.id,
               edge2.p2.id);
    return std::nullopt;
}

BDSPointHandle
BDS_Mesh::opposite_vertex(BDSEdgeHandle eh, const std::array<BDSPointHandle, 3> & pts) const
{
    const auto & edge = get_edge(eh);
    if (pts[0] != edge.p1 && pts[0] != edge.p2)
        return pts[0];
    else if (pts[1] != edge.p1 && pts[1] != edge.p2)
        return pts[1];
    else
        return pts[2];
}

Optional<BDSPointHandle>
BDS_Mesh::opposite_vertex(BDSFaceHandle fh, BDSEdgeHandle eh) const
{
    const auto & face = get_face(fh);

    if (eh == face.e1)
        return common_vertex(face.e2, face.e3);
    if (eh == face.e2)
        return common_vertex(face.e1, face.e3);
    if (eh == face.e3)
        return common_vertex(face.e1, face.e2);
    const auto & edge = get_edge(eh);
    Log::error("Edge {} {} does not belong to this triangle", edge.p1.id, edge.p2.id);
    return std::nullopt;
}

Optional<BDSEdgeHandle>
BDS_Mesh::add_edge(BDSPointHandle ph1, BDSPointHandle ph2)
{
    auto efound = find_edge(ph1, ph2);
    if (efound.has_value())
        return efound;

    if (ph1.is_null() || ph2.is_null())
        return std::nullopt;

    this->edges_.emplace_back(ph1, ph2);
    BDSEdgeHandle eh { static_cast<u32>(this->edges_.size() - 1) };
    get_point(ph1).edges.push_back(eh);
    get_point(ph2).edges.push_back(eh);
    return eh;
}

Optional<BDSEdgeHandle>
BDS_Mesh::find_edge(BDSPointHandle ph1, BDSPointHandle ph2) const
{
    const auto & p1 = get_point(ph1);
    for (const auto eh : p1.edges) {
        const auto & edge = get_edge(eh);
        if (edge.p1 == ph1 && edge.p2 == ph2)
            return eh;
        if (edge.p2 == ph1 && edge.p1 == ph2)
            return eh;
    }
    return std::nullopt;
}

Optional<BDSEdgeHandle>
BDS_Mesh::find_edge(BDSPointHandle ph1, BDSPointHandle ph2, BDSFaceHandle fh) const
{
    const auto & tri = get_face(fh);
    // note see BDS_Edge::BDS_Edge why we "sort"
    if (ph1 >= ph2)
        std::swap(ph1, ph2);
    const auto & edge1 = get_edge(tri.e1);
    if (edge1.p1 == ph1 && edge1.p2 == ph2)
        return tri.e1;
    const auto & edge2 = get_edge(tri.e2);
    if (edge2.p1 == ph1 && edge2.p2 == ph2)
        return tri.e2;
    const auto & edge3 = get_edge(tri.e3);
    if (edge3.p1 == ph1 && edge3.p2 == ph2)
        return tri.e3;
    return std::nullopt;
}

void
BDS_Mesh::del_point(BDSPointHandle ph)
{
    if (ph.is_null())
        return;
    auto & p = get_point(ph);
    this->points_by_num_.erase(p.id);
}

u32
BDS_Mesh::num_faces(BDSEdgeHandle eh) const
{
    return static_cast<u32>(get_edge(eh).faces.size());
}

std::array<BDSPointHandle, 2>
BDS_Mesh::opposite_of(BDSEdgeHandle eh) const
{
    std::array<BDSPointHandle, 2> oface;
    const auto & edge = get_edge(eh);
    if (edge.faces.size() > 0 && not edge.faces[0].is_null()) {
        auto pts_res = get_nodes(edge.faces[0]);
        if (not pts_res.has_value())
            return {};
        oface[0] = opposite_vertex(eh, pts_res.value());
    }
    if (edge.faces.size() > 1 && not edge.faces[1].is_null()) {
        auto pts_res = get_nodes(edge.faces[1]);
        if (not pts_res.has_value())
            return {};
        oface[1] = opposite_vertex(eh, pts_res.value());
    }
    return oface;
}

Optional<BDSPointHandle>
BDS_Mesh::other_vertex(BDSEdgeHandle eh, BDSPointHandle ph) const
{
    const auto & edge = get_edge(eh);

    if (edge.p1 == ph)
        return edge.p2;
    if (edge.p2 == ph)
        return edge.p1;
    Log::error("Edge {}-{} does not contain node {}", edge.p1.id, edge.p2.id, ph.id);
    return std::nullopt;
}

Optional<BDSFaceHandle>
BDS_Mesh::other_face(BDSEdgeHandle eh, BDSFaceHandle fh) const
{
    const auto & edge = get_edge(eh);

    if (num_faces(eh) != 2) {
        Log::error("{} face(s) attached to edge {}-{}", num_faces(eh), edge.p1.id, edge.p2.id);
        return std::nullopt;
    }
    if (fh == edge.faces[0])
        return edge.faces[1];
    if (fh == edge.faces[1])
        return edge.faces[0];
    Log::error("Edge {}-{} does not belong to the face", edge.p1.id, edge.p2.id);
    return std::nullopt;
}

std::tuple<Optional<std::array<BDSPointHandle, 3>>,
           Optional<std::array<BDSPointHandle, 3>>,
           std::array<Optional<BDSPointHandle>, 2>>
BDS_Mesh::compute_neighborhood(BDSEdgeHandle eh) const
{
    const auto & edge = get_edge(eh);

    std::array<Optional<BDSPointHandle>, 2> oface;
    Optional<std::array<BDSPointHandle, 3>> pts1;
    Optional<std::array<BDSPointHandle, 3>> pts2;
    if (not edge.faces[0].is_null()) {
        auto pts_res = get_nodes(edge.faces[0]);
        if (not pts_res.has_value())
            return { pts1, pts2, oface };
        oface[0] = opposite_vertex(eh, pts_res.value());
        pts1 = pts_res.value();
    }
    if (not edge.faces[1].is_null()) {
        auto pts_res = get_nodes(edge.faces[1]);
        if (not pts_res.has_value())
            return { pts1, pts2, oface };
        oface[1] = opposite_vertex(eh, pts_res.value());
        pts2 = pts_res.value();
    }
    return { pts1, pts2, oface };
}

void
BDS_Mesh::del_edge(BDSEdgeHandle eh)
{
    if (eh.is_null())
        return;
    auto & edge = get_edge(eh);
    get_point(edge.p1).del(eh);
    get_point(edge.p2).del(eh);
    edge.active = false;
}

Optional<BDSFaceHandle>
BDS_Mesh::add_face(BDSPointHandle ph1,
                   BDSPointHandle ph2,
                   BDSPointHandle ph3,
                   Optional<BDS_GeomEntity> ge)
{
    auto eh1 = add_edge(ph1, ph2);
    auto eh2 = add_edge(ph2, ph3);
    auto eh3 = add_edge(ph3, ph1);
    if (eh1.has_value() && eh2.has_value() && eh3.has_value())
        return add_face(eh1.value(), eh2.value(), eh3.value(), ge);
    return std::nullopt;
}

Optional<BDSFaceHandle>
BDS_Mesh::add_face(BDSEdgeHandle eh1,
                   BDSEdgeHandle eh2,
                   BDSEdgeHandle eh3,
                   Optional<BDS_GeomEntity> ge)
{
    if (eh1.is_null() || eh2.is_null() || eh3.is_null())
        return std::nullopt;

    this->triangles_.emplace_back(eh1, eh2, eh3, ge);
    BDSFaceHandle fh { static_cast<u32>(this->triangles_.size() - 1) };
    get_edge(eh1).faces.push_back(fh);
    get_edge(eh2).faces.push_back(fh);
    get_edge(eh3).faces.push_back(fh);
    return fh;
}

Optional<BDSEdgeHandle>
BDS_Mesh::opposite_edge(BDSFaceHandle fh, BDSPointHandle ph)
{
    const auto & face = get_face(fh);
    const auto & edge1 = get_edge(face.e1);
    const auto & edge2 = get_edge(face.e2);
    const auto & edge3 = get_edge(face.e3);

    if (edge1.p1 != ph && edge1.p2 != ph)
        return face.e1;
    if (edge2.p1 != ph && edge2.p2 != ph)
        return face.e2;
    if (edge3.p1 != ph && edge3.p2 != ph)
        return face.e3;
    Log::error("Point {} does not belong to this triangle", ph.id);
    return std::nullopt;
}

void
BDS_Mesh::del_face(BDSFaceHandle th)
{
    if (th.is_null())
        return;
    auto & tri = get_face(th);
    get_edge(tri.e1).active = false;
    get_edge(tri.e2).active = false;
    get_edge(tri.e3).active = false;
    tri.active = false;
}

void
BDS_Mesh::set_lc(BDSPointHandle ph, double lc)
{
    get_point(ph).lc = lc;
}

Optional<BDSPointHandle>
BDS_Mesh::find_point(int idx) const
{
    auto it = this->points_by_num_.find(idx);
    if (it != this->points_by_num_.end())
        return it->second;
    else
        return std::nullopt;
}

void
BDS_Mesh::set_ge(BDSEdgeHandle eh, Optional<BDS_GeomEntity> ge)
{
    get_edge(eh).ge = ge;
}

BDS_GeomEntity
BDS_Mesh::add_geom(int tag, int degree)
{
    auto [it, inserted] = this->geom_.emplace(tag, degree);
    return *it;
}

Optional<BDSEdgeHandle>
BDS_Mesh::recover_edge(BDSPointHandle ph1,
                       BDSPointHandle ph2,
                       bool & fatal,
                       std::set<EdgeToRecover> * e2r,
                       std::set<EdgeToRecover> * not_recovered)
{
    auto eh = find_edge(ph1, ph2);
    fatal = false;

    if (eh.has_value())
        return eh.value();

    Log::debug("Edge {} {} has to be recovered", ph1.id, ph2.id);

    int ix = 0;
    while (true) {
        std::vector<BDSEdgeHandle> intersected;

        bool self_intersection = false;

        for (const auto eh : edges()) {
            auto & e = get_edge(eh);
            if (e.active && e.p1 != ph1 && e.p1 != ph2 && e.p2 != ph1 && e.p2 != ph2) {
                const auto & p1 = get_point(e.p1);
                const auto & p2 = get_point(e.p2);
                const auto & q1 = get_point(ph1);
                const auto & q2 = get_point(ph2);
                if (intersect_edges_2d(p1.uv, p2.uv, q1.uv, q2.uv)) {
                    // intersect
                    if (e2r && e2r->contains(EdgeToRecover(e.p1, e.p2))) {
                        // Msg::Debug("edge %d %d on model edge %d cannot be recovered because"
                        //            " it intersects %d %d on model edge %d",
                        //            num1,
                        //            num2,
                        //            itr2->ge->tag(),
                        //            e->p1->iD,
                        //            e->p2->iD,
                        //            itr1->ge->tag());
                        // now throw a class that contains the diagnostic
                        not_recovered->insert(EdgeToRecover(ph1, ph2));
                        not_recovered->insert(EdgeToRecover(e.p1, e.p2));
                        self_intersection = true;
                    }
                    intersected.push_back(eh);
                }
            }
        }

        if (self_intersection)
            return std::nullopt;

        if (intersected.empty() || ix > 300) {
            auto eee = find_edge(ph1, ph2);
            if (not eee.has_value()) {
                // if (Msg::GetVerbosity() > 98) {
                //     outputScalarField(triangles, "debugp.pos", 1);
                //     outputScalarField(triangles, "debugr.pos", 0);
                //     Msg::Debug("edge %d %d cannot be recovered at all, look at debugp.pos "
                //                "and debugr.pos",
                //                num1,
                //                num2);
                // }
                // else {
                //     Msg::Debug("edge %d %d cannot be recovered at all", num1, num2);
                // }
                fatal = true;
                return std::nullopt;
            }
            return eee.value();
        }

        std::vector<int>::size_type ichoice = 0;
        bool success = false;
        while (!success && ichoice < intersected.size()) {
            success = swap_edge(intersected[ichoice++], BDS_SwapEdgeTestRecover(*this));
        }

        if (!success) {
            // Msg::Debug("edge %d %d cannot be recovered at all\n", num1, num2);
            fatal = true;
            return std::nullopt;
        }

        ix++;
    }
    return std::nullopt;
}

Optional<BDSEdgeHandle>
BDS_Mesh::recover_edge_fast(BDSPointHandle ph1, BDSPointHandle ph2)
{
    for (const auto th : triangles(ph1)) {
        auto edge = opposite_edge(th, ph1);
        // NOTE: promote the assert into runtime error
        assert(edge.has_value());
        auto face = other_face(edge.value(), th);
        // NOTE: promote the assert into runtime error
        assert(face.has_value());
        auto p2b = opposite_vertex(face.value(), edge.value());
        if (p2b.has_value() && ph2 == p2b.value()) {
            if (swap_edge(edge.value(), BDS_SwapEdgeTestRecover(*this), true))
                return find_edge(ph1, ph2);
        }
    }
    return std::nullopt;
}

bool
BDS_Mesh::swap_edge(BDSEdgeHandle eh, const BDS_SwapEdgeTest & theTest, bool force)
{
    /*
          p1
        / | \
       /  |  \
   op1/ 0 | 1 \op2
      \   |   /
       \  |  /
        \ p2/

       // op1 p1 op2
       // op1 op2 p2
     */

    // we test if the edge is deleted
    // return false;

    const auto & e = get_edge(eh);

    if (not e.active)
        return false;

    if (num_faces(eh) != 2)
        return false;

    if (e.ge && e.ge.value().degree == 1)
        return false;

    auto [pts1, pts2, oph] = compute_neighborhood(eh);
    if (!oph[0] || !oph[1])
        return false;
    auto ph1 = e.p1;
    auto ph2 = e.p2;
    auto oph0 = oph[0].value();
    auto oph1 = oph[1].value();

    const auto & p1 = get_point(ph1);
    const auto & p2 = get_point(ph2);
    const auto & q1 = get_point(oph0);
    const auto & q2 = get_point(oph1);

    if (!force && !p1.config_modified && !p2.config_modified && !q1.config_modified &&
        !q2.config_modified)
        return false;

    std::array<Optional<BDS_GeomEntity>, 2> g;

    // compute the orientation of the face
    // with respect to the edge
    int orientation = 0;
    for (int i = 0; i < 3; i++) {
        if (pts1.value()[i] == ph1) {
            orientation = pts1.value()[(i + 1) % 3] == ph2 ? 1 : -1;
            break;
        }
    }

    if (orientation == 1) {
        if (!theTest(ph1, ph2, oph0, ph2, ph1, oph1, ph1, oph1, oph0, oph1, ph2, oph0))
            return false;
    }
    else {
        if (!theTest(ph2, ph1, oph0, ph1, ph2, oph1, ph1, oph0, oph1, oph1, oph0, ph2))
            return false;
    }

    if (!theTest(ph1, ph2, oph0, oph1))
        return false;

    // auto & faces = e.faces;
    auto p1_op1 = find_edge(ph1, oph0, e.faces[0]);
    auto op1_p2 = find_edge(oph0, ph2, e.faces[0]);
    auto p1_op2 = find_edge(ph1, oph1, e.faces[1]);
    auto op2_p2 = find_edge(oph1, ph2, e.faces[1]);

    // degenerate
    if (p1_op1.value() == p1_op2.value() || op2_p2.value() == op1_p2.value())
        return false;

    for (auto i : { 0, 1 }) {
        if (not e.faces[i].is_null()) {
            g[i] = get_face(e.faces[i]).ge;
            del_face(e.faces[i]);
        }
    }
    del_edge(eh);

    this->edges_.emplace_back(oph0, oph1, e.ge);
    BDSEdgeHandle ref_edge { static_cast<u32>(this->edges_.size() - 1) };

    if (orientation == 1) {
        add_face(p1_op1.value(), p1_op2.value(), ref_edge, g[0]);
        add_face(ref_edge, op2_p2.value(), op1_p2.value(), g[1]);
    }
    else {
        add_face(p1_op2.value(), p1_op1.value(), ref_edge, g[0]);
        add_face(op2_p2.value(), ref_edge, op1_p2.value(), g[1]);
    }

    get_point(ph1).config_modified = true;
    get_point(ph2).config_modified = true;
    get_point(oph0).config_modified = true;
    get_point(oph1).config_modified = true;

    return true;
}

bool
BDS_Mesh::collapse_edge_parametric(BDSEdgeHandle eh, BDSPointHandle ph, bool force)
{
    const auto & p = get_point(ph);
    const auto & e = get_edge(eh);

    if (!force && num_faces(eh) != 2)
        return false;
    if (!force && p.ge && p.ge->degree == 0)
        return false;
    // not really ok but 'til now this is the best choice not to do collapses on
    // model edges
    if (!force && p.ge && p.ge->degree == 1)
        return false;
    if (!force && e.ge && p.ge) {
        if (e.ge->degree == 2 && p.ge != e.ge)
            return false;
    }

    if (!force) {
        auto & ep1 = get_point(e.p1);
        auto & ep2 = get_point(e.p2);
        for (std::size_t i = 0; i < ep1.edges.size(); i++) {
            for (std::size_t j = 0; j < ep2.edges.size(); j++) {
                auto & edge_a = get_edge(ep1.edges[i]);
                auto & edge_b = get_edge(ep2.edges[j]);

                auto p1 = edge_a.p1 == e.p1 ? edge_a.p2 : edge_a.p1;
                auto p2 = edge_b.p1 == e.p2 ? edge_b.p2 : edge_b.p1;
                if (get_point(p1).periodic_counterpart == p2)
                    return false;
            }
        }
    }

    if (num_faces(eh) == 2) {
        auto ofaceh = opposite_of(eh);
        if (ofaceh[0].is_null() || ofaceh[1].is_null()) {
            Log::error("No opposite face in edge collapse");
            return false;
        }
        const auto & oface0 = get_point(ofaceh[0]);
        const auto & oface1 = get_point(ofaceh[1]);
        for (std::size_t i = 0; i < oface0.edges.size(); i++) {
            const auto & edge = get_edge(oface0.edges[i]);
            if (edge.p1 == ofaceh[0] && edge.p2 == ofaceh[1])
                return false;
            if (edge.p1 == ofaceh[1] && edge.p2 == ofaceh[0])
                return false;
        }
        if (!force && oface0.ge && oface0.ge->degree == 2 && oface0.edges.size() <= 4)
            return false;
        if (!force && oface1.ge && oface1.ge->degree == 2 && oface1.edges.size() <= 4)
            return false;
        if (!force && oface0.ge && oface0.ge->degree < 2 && oface0.edges.size() <= 3)
            return false;
        if (!force && oface1.ge && oface1.ge->degree < 2 && oface1.edges.size() <= 3)
            return false;
    }
    auto o = other_vertex(eh, ph);

    std::vector<std::array<BDSPointHandle, 3>> pt;
    std::vector<Optional<BDS_GeomEntity>> gs;
    pt.reserve(1024);
    gs.reserve(1024);

    double area_old = 0.0;
    double area_new = 0.0;
    for (const auto th : triangles(ph)) {
        auto pts_res = get_nodes(th);
        if (pts_res.has_value()) {
            const auto & t = get_face(th);
            auto pts = pts_res.value();
            double sold = std::abs(surface_triangle_param(pts[0], pts[1], pts[2]));
            area_old += sold;
            if (t.e1 != eh && t.e2 != eh && t.e3 != eh) {
                std::array<Optional<BDSPointHandle>, 3> pot_tri = {
                    (pts[0] == ph) ? o : pts[0],
                    (pts[1] == ph) ? o : pts[1],
                    (pts[2] == ph) ? o : pts[2],
                };
                if (not pot_tri[0].has_value() || not pot_tri[1].has_value() ||
                    not pot_tri[2].has_value()) {
                    return false;
                }
                std::array<BDSPointHandle, 3> tri = { pot_tri[0].value(),
                                                      pot_tri[1].value(),
                                                      pot_tri[2].value() };
                double snew = std::abs(surface_triangle_param(tri[0], tri[1], tri[2]));
                if (!force && snew < .02 * sold) {
                    return false;
                }
                area_new += snew;
                pt.emplace_back(tri);
                gs.emplace_back(t.ge);
            }
        }
    }

    if (!force && std::abs(area_old - area_new) > 1.e-12 * (area_old + area_new)) {
        return false;
    }
    {
        for (const auto tri : triangles(ph))
            del_face(tri);
    }

    std::vector<std::array<BDSPointHandle, 2>> ept;
    std::vector<Optional<BDS_GeomEntity>> egs;
    ept.reserve(1024);
    egs.reserve(1024);
    {
        auto calc_ept =
            [](BDSPointHandle a, BDSPointHandle p, Optional<BDSPointHandle> o) -> BDSPointHandle {
            if (a == p) {
                if (o.has_value())
                    return o.value();
                else
                    return {};
            }
            else {
                return a;
            }
        };

        std::vector<BDSEdgeHandle> edges(p.edges);
        for (const auto ehi : edges) {
            const auto & edge = get_edge(ehi);
            get_point(edge.p1).config_modified = true;
            get_point(edge.p2).config_modified = true;
            std::array<BDSPointHandle, 2> aaa = { calc_ept(edge.p1, ph, o),
                                                  calc_ept(edge.p2, ph, o) };
            if (aaa[0].is_null() || aaa[1].is_null()) {
                return false;
            }
            ept.emplace_back(aaa);
            egs.emplace_back(edge.ge);
            del_edge(ehi);
        }
    }

    del_point(ph);

    for (std::size_t i = 0; i < pt.size(); i++) {
        auto & tri = pt[i];
        add_face(tri[0], tri[1], tri[1], gs[i]);
    }

    for (std::size_t i = 0; i < ept.size(); ++i) {
        auto e = find_edge(ept[i][0], ept[i][1]);
        if (e.has_value()) {
            auto & ee = get_edge(e.value());
            if (not ee.ge) {
                ee.ge = egs[i];
            }
        }
    }

    return true;
}

void
BDS_Mesh::cleanup()
{
    // NOTE: this is noop, since it would break all indexing
}

std::vector<BDSFaceHandle>
BDS_Mesh::triangles(BDSPointHandle ph) const
{
    const auto & pt = get_point(ph);

    std::vector<BDSFaceHandle> t;
    t.reserve(pt.edges.size());

    for (const auto & eh : pt.edges) {
        const auto & edge = get_edge(eh);
        for (const auto & fh : edge.faces) {
            if (std::find(t.begin(), t.end(), fh) == t.end()) {
                t.push_back(fh);
            }
        }
    }
    return t;
}

bool
BDS_Mesh::validity_of_cavity(UVParam p, const std::vector<BDSPointHandle> & nbgh)
{
    const auto & nbg0 = get_point(nbgh[0]);
    const auto & nbg1 = get_point(nbgh[1]);
    UVParam q = { nbg0.degenerated == 1 ? nbg1.u() : nbg0.u(),
                  nbg0.degenerated == 2 ? nbg1.v() : nbg0.v() };
    UVParam r = { nbg1.degenerated == 1 ? nbg0.u() : nbg1.u(),
                  nbg1.degenerated == 2 ? nbg0.v() : nbg1.v() };
    auto sign = orient2d(p, q, r);
    for (size_t i = 1; i < nbgh.size(); ++i) {
        const auto & p0 = get_point(nbgh[i]);
        const auto & p1 = get_point(nbgh[(i + 1) % nbgh.size()]);
        UVParam qq = { p0.degenerated == 1 ? p1.u() : p0.u(),
                       p0.degenerated == 2 ? p1.v() : p0.v() };
        UVParam rr = { p1.degenerated == 1 ? p0.u() : p1.u(),
                       p1.degenerated == 2 ? p0.v() : p1.v() };
        auto s_sign = orient2d(p, qq, rr);
        if (sign * s_sign <= 0)
            return false;
    }
    return true;
}

std::tuple<double, double>
BDS_Mesh::tutte_energy(Point pt, const std::vector<BDSPointHandle> & nbgh)
{
    if (nbgh.empty())
        return { MAX_LC, 0. };
    double E = 0;
    double maximum = 0., minimum = 0.;
    for (size_t i = 0; i < nbgh.size(); ++i) {
        auto & nbg_pt = get_point(nbgh[i]);
        const auto delta = (pt - nbg_pt.point);
        const auto l2 = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
        maximum = i ? std::max(maximum, l2) : l2;
        minimum = i ? std::min(minimum, l2) : l2;
        E += l2;
    }
    if (!maximum)
        return { MAX_LC, 0 };
    double ratio = minimum / maximum;
    return { E, ratio };
}

bool
BDS_Mesh::minimize_tutte_energy_proj(BDSPointHandle ph,
                                     double E_unmoved,
                                     const std::vector<BDSPointHandle> & nbgh,
                                     const std::vector<UVParam> & kernel,
                                     const std::vector<double> & lc,
                                     const GeomSurface & gf)
{
    Point x;
    double sum = 0.;
    auto & p0 = get_point(ph).point;
    for (std::size_t i = 0; i < nbgh.size(); ++i) {
        auto & pi = get_point(nbgh[i]).point;
        auto & pip = get_point(nbgh[(i + 1) % nbgh.size()]).point;
        auto v1 = pi - p0;
        auto v2 = pip - p0;
        auto pv = cross_product(v1, v2);
        auto nrm = pv.magnitude();
        x += (pi + p0 + pip) * (nrm / 3.0);
        sum += nrm;
    }
    x /= sum;

    auto [ctr_uv, _] = centroid_uv(kernel, lc);
    auto [gp, uv] = gf.closest_point(x, ctr_uv);
    if (validity_of_cavity(uv, nbgh)) {
        auto [E_moved, _] = tutte_energy(gp, nbgh);
        if (E_moved < E_unmoved)
            return true;
    }
    return false;
}

bool
BDS_Mesh::minimize_tutte_energy_param(BDSPointHandle ph,
                                      double E_unmoved,
                                      const std::vector<BDSPointHandle> & nbg,
                                      const std::vector<UVParam> & kernel,
                                      const std::vector<double> & lcs,
                                      const GeomSurface & gf)
{
    auto & p = get_point(ph);
    auto [uv, LC] = centroid_uv(p, gf, kernel, lcs);
    auto gp = gf.point(uv);
    auto [E_moved, ratio2] = tutte_energy(gp, nbg);
    if (E_moved < E_unmoved) {
        if (!validity_of_cavity(uv, nbg))
            return false;
        p.lc = LC;
        return ratio2 > .25;
    }
    return false;
}

std::tuple<std::vector<UVParam>, std::vector<double>>
BDS_Mesh::compute_some_kind_of_kernel(BDSPointHandle ph, const std::vector<BDSPointHandle> & nbgh)
{
    std::vector<UVParam> kernels;
    std::vector<double> lcs;

    const auto & p = get_point(ph);
    auto pp = p.uv;
    auto ll = p.lc;
    for (std::size_t i = 0; i < nbgh.size(); i++) {
        auto & nbgi = get_point(nbgh[i]);
        auto & nbgj = get_point(nbgh[(i + 1) % nbgh.size()]);
        if (nbgi.degenerated == 1) {
            kernels.emplace_back(p.u(), nbgi.v());
            kernels.emplace_back(nbgj.u(), nbgi.v());

            lcs.push_back(nbgi.lc);
            lcs.push_back(nbgi.lc);
        }
        else if (nbgi.degenerated == 2) {
            kernels.emplace_back(nbgi.u(), p.v());
            kernels.emplace_back(nbgi.u(), nbgj.v());

            lcs.push_back(nbgi.lc);
            lcs.push_back(nbgi.lc);
        }
        else if (nbgj.degenerated == 1) {
            kernels.emplace_back(nbgi.u(), nbgi.v());
            kernels.emplace_back(nbgi.u(), nbgj.v());
            lcs.push_back(nbgi.lc);
            lcs.push_back(nbgi.lc);
        }
        else if (nbgj.degenerated == 2) {
            kernels.emplace_back(nbgi.u(), nbgi.v());
            kernels.emplace_back(nbgj.u(), nbgi.v());
            lcs.push_back(nbgi.lc);
            lcs.push_back(nbgi.lc);
        }
        else {
            kernels.emplace_back(nbgi.u(), nbgi.v());
            lcs.push_back(nbgi.lc);
        }
    }

    // we should compute the true kernel
    for (std::size_t i = 0; i < kernels.size(); i++) {
        auto p_now = kernels[i];
        double lc_now = lcs[i];
        for (size_t j = 0; j < kernels.size(); j++) {
            if (i != j && i != (j + 1) % kernels.size()) {
                const auto p0 = kernels[j];
                const auto p1 = kernels[(j + 1) % kernels.size()];
                auto x = intersection(pp, p_now, p0, p1);
                if (x[0] > 0 && x[0] < 1.0) {
                    p_now = (pp * (1. - x[0])) + (p_now * x[0]);
                    lc_now = ll * (1. - x[0]) + lc_now * x[0];
                }
            }
        }
        kernels[i] = p_now;
        lcs[i] = lc_now;
    }

    return { kernels, lcs };
}

double
BDS_Mesh::surface_triangle_param(BDSPointHandle ph1, BDSPointHandle ph2, BDSPointHandle ph3)
{
    auto p1 = cref(get_point(ph1));
    auto p2 = cref(get_point(ph2));
    auto p3 = cref(get_point(ph3));
    return surface_triangle_param2(p1, p2, p3);
}

//

BDS_SwapEdgeTestRecover::BDS_SwapEdgeTestRecover(const BDS_Mesh & mesh) : BDS_SwapEdgeTest(mesh) {}

bool
BDS_SwapEdgeTestRecover::operator()(BDSPointHandle ph1,
                                    BDSPointHandle ph2,
                                    BDSPointHandle qh1,
                                    BDSPointHandle qh2) const
{
    const auto & p1 = this->mesh_.get_point(ph1);
    const auto & p2 = this->mesh_.get_point(ph2);
    const auto & q1 = this->mesh_.get_point(qh1);
    const auto & q2 = this->mesh_.get_point(qh2);

    auto ori_t1 = orient2d(q1.uv, p1.uv, q2.uv);
    auto ori_t2 = orient2d(q1.uv, q2.uv, p2.uv);
    return (ori_t1 * ori_t2 > 0); // the quadrangle was strictly convex !
}

bool
BDS_SwapEdgeTestRecover::operator()(BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle,
                                    BDSPointHandle) const
{
    return true;
}

// This function does actually the swap without taking into account
// the feasability of the operation. Those conditions have to be
// taken into account before doing the edge swap

BDS_SwapEdgeTestQuality::BDS_SwapEdgeTestQuality(const BDS_Mesh & mesh, bool a, bool b) :
    BDS_SwapEdgeTest(mesh),
    test_quality_(a),
    test_small_triangles_(b)
{
}

bool
BDS_SwapEdgeTestQuality::operator()(BDSPointHandle ph1,
                                    BDSPointHandle ph2,
                                    BDSPointHandle qh1,
                                    BDSPointHandle qh2) const
{
    if (!this->test_small_triangles_)
        return true;

    const auto & p1 = cref(this->mesh_.get_point(ph1));
    const auto & p2 = cref(this->mesh_.get_point(ph2));
    const auto & q1 = cref(this->mesh_.get_point(qh1));
    const auto & q2 = cref(this->mesh_.get_point(qh2));

    // AVOID CREATING POINTS WITH 2 NEIGHBORING TRIANGLES
    //  std::vector<BDS_Face*> f1 = p1->getTriangles();
    //  std::vector<BDS_Face*> f2 = p2->getTriangles();
    if (p1->ge && p1->ge->degree == 2 && p1->edges.size() <= 4)
        return false;
    if (p2->ge && p2->ge->degree == 2 && p2->edges.size() <= 4)
        return false;
    if (p1->ge && p1->ge->degree < 2 && p1->edges.size() <= 3)
        return false;
    if (p2->ge && p2->ge->degree < 2 && p2->edges.size() <= 3)
        return false;

    auto s1 = std::abs(surface_triangle_param2(p1, p2, q1));
    auto s2 = std::abs(surface_triangle_param2(p1, p2, q2));
    auto s3 = std::abs(surface_triangle_param2(p1, q1, q2));
    auto s4 = std::abs(surface_triangle_param2(p2, q1, q2));
    if (std::abs(s1 + s2 - s3 - s4) > 1.e-12 * (s3 + s4))
        return false;
    else
        return true;
}

bool
BDS_SwapEdgeTestQuality::operator()(BDSPointHandle ph1,
                                    BDSPointHandle ph2,
                                    BDSPointHandle ph3,
                                    BDSPointHandle qh1,
                                    BDSPointHandle qh2,
                                    BDSPointHandle qh3,
                                    BDSPointHandle oph1,
                                    BDSPointHandle oph2,
                                    BDSPointHandle oph3,
                                    BDSPointHandle oqh1,
                                    BDSPointHandle oqh2,
                                    BDSPointHandle oqh3) const
{
    // Check if new edge is not on a seam or degenerated
    std::array<BDSPointHandle, 2> ptsh;
    if (oph1 != oqh1 && oph1 != oqh2 && oph1 != oqh3) {
        ptsh = { oph2, oph3 };
    }
    else if (oph2 != oqh1 && oph2 != oqh2 && oph2 != oqh3) {
        ptsh = { oph1, oph3 };
    }
    else if (oph3 != oqh1 && oph3 != oqh2 && oph3 != oqh3) {
        ptsh = { oph1, oph2 };
    }
    else {
        Log::warn("Unable to detect the new edge in BDS_SwapEdgeTestQuality");
    }

    if (not ptsh[0].is_null() && not ptsh[1].is_null()) {
        const auto & pts0 = this->mesh_.get_point(ptsh[0]);
        const auto & pts1 = this->mesh_.get_point(ptsh[1]);
        if (pts0.degenerated && pts1.degenerated)
            return false;
        if (not pts0.periodic_counterpart.is_null() && not pts1.periodic_counterpart.is_null())
            return false;
    }

    if (!this->test_quality_)
        return true;

    const auto & p1 = this->mesh_.get_point(ph1);
    const auto & p2 = this->mesh_.get_point(ph2);
    const auto & p3 = this->mesh_.get_point(ph3);
    const auto & q1 = this->mesh_.get_point(qh1);
    const auto & q2 = this->mesh_.get_point(qh2);
    const auto & q3 = this->mesh_.get_point(qh3);
    const auto & op1 = this->mesh_.get_point(oph1);
    const auto & op2 = this->mesh_.get_point(oph2);
    const auto & op3 = this->mesh_.get_point(oph3);
    const auto & oq1 = this->mesh_.get_point(oqh1);
    const auto & oq2 = this->mesh_.get_point(oqh2);
    const auto & oq3 = this->mesh_.get_point(oqh3);

    const auto qa1 = Tri3::gamma(p1.point, p2.point, p3.point);
    const auto qa2 = Tri3::gamma(q1.point, q2.point, q3.point);
    const auto qb1 = Tri3::gamma(op1.point, op2.point, op3.point);
    const auto qb2 = Tri3::gamma(oq1.point, oq2.point, oq3.point);

    // we swap for a better configuration
    const auto mina = std::min(qa1, qa2);
    const auto minb = std::min(qb1, qb2);

    return minb > mina;
}

BDS_SwapEdgeTestNormals::BDS_SwapEdgeTestNormals(const BDS_Mesh & mesh,
                                                 GeomSurface * gf,
                                                 double ori) :
    BDS_SwapEdgeTest(mesh),
    gf_(gf),
    ori_(ori)
{
}

bool
BDS_SwapEdgeTestNormals::operator()(BDSPointHandle ph1,
                                    BDSPointHandle ph2,
                                    BDSPointHandle qh1,
                                    BDSPointHandle qh2) const
{
    auto p1 = cref(this->mesh_.get_point(ph1));
    auto p2 = cref(this->mesh_.get_point(ph2));
    auto q1 = cref(this->mesh_.get_point(qh1));
    auto q2 = cref(this->mesh_.get_point(qh2));

    auto s1 = std::abs(surface_triangle_param2(p1, p2, q1));
    auto s2 = std::abs(surface_triangle_param2(p1, p2, q2));
    auto s3 = std::abs(surface_triangle_param2(p1, q1, q2));
    auto s4 = std::abs(surface_triangle_param2(p2, q1, q2));
    if (std::abs(s1 + s2 - s3 - s4) > 1.e-12 * (s3 + s4)) {
        return false;
    }
    return true;
}

bool
BDS_SwapEdgeTestNormals::operator()(BDSPointHandle ph1,
                                    BDSPointHandle ph2,
                                    BDSPointHandle ph3,
                                    BDSPointHandle qh1,
                                    BDSPointHandle qh2,
                                    BDSPointHandle qh3,
                                    BDSPointHandle oph1,
                                    BDSPointHandle oph2,
                                    BDSPointHandle oph3,
                                    BDSPointHandle oqh1,
                                    BDSPointHandle oqh2,
                                    BDSPointHandle oqh3) const
{
    const auto & p1 = this->mesh_.get_point(ph1);
    const auto & p2 = this->mesh_.get_point(ph2);
    const auto & p3 = this->mesh_.get_point(ph3);
    const auto & q1 = this->mesh_.get_point(qh1);
    const auto & q2 = this->mesh_.get_point(qh2);
    const auto & q3 = this->mesh_.get_point(qh3);
    const auto & op1 = this->mesh_.get_point(oph1);
    const auto & op2 = this->mesh_.get_point(oph2);
    const auto & op3 = this->mesh_.get_point(oph3);
    const auto & oq1 = this->mesh_.get_point(oqh1);
    const auto & oq2 = this->mesh_.get_point(oqh2);
    const auto & oq3 = this->mesh_.get_point(oqh3);

    auto qa1 = Tri3::gamma(p1.point, p2.point, p3.point);
    auto qa2 = Tri3::gamma(q1.point, q2.point, q3.point);
    auto qb1 = Tri3::gamma(op1.point, op2.point, op3.point);
    auto qb2 = Tri3::gamma(oq1.point, oq2.point, oq3.point);

    double OLD = std::min(this->ori_ * qa1 * _cos_N(p1, p2, p3, this->gf_),
                          this->ori_ * qa2 * _cos_N(q1, q2, q3, this->gf_));
    double NEW = std::min(this->ori_ * qb1 * _cos_N(op1, op2, op3, this->gf_),
                          this->ori_ * qb2 * _cos_N(oq1, oq2, oq3, this->gf_));

    if (OLD < 0.5 && OLD < NEW)
        return true;
    return false;
}

void
BDS_Mesh::recur_tag(BDSFaceHandle th, BDS_GeomEntity ge)
{
    std::stack<BDSFaceHandle> stack;
    stack.push(th);

    while (!stack.empty()) {
        th = stack.top();
        stack.pop();
        auto & tri = get_face(th);
        if (not tri.ge.has_value()) {
            tri.ge = ge;
            auto & e1 = get_edge(tri.e1);
            if (not e1.ge.has_value() && num_faces(tri.e1) == 2) {
                stack.push(other_face(tri.e1, th).value());
            }
            auto & e2 = get_edge(tri.e2);
            if (not e2.ge.has_value() && num_faces(tri.e2) == 2) {
                stack.push(other_face(tri.e2, th).value());
            }
            auto & e3 = get_edge(tri.e3);
            if (not e3.ge.has_value() && num_faces(tri.e3) == 2) {
                stack.push(other_face(tri.e3, th).value());
            }
        }
    }
}

} // namespace krado
