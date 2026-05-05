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

namespace krado {

namespace {

Vector
vector_triangle(const BDS_Point * p1, const BDS_Point * p2, const BDS_Point * p3)
{
    auto a = p1->point() - p2->point();
    auto b = p1->point() - p3->point();
    return cross_product(a, b);
}

double
vector_triangle_parametric(const BDS_Point * p1, const BDS_Point * p2, const BDS_Point * p3)
{
    auto a = p1->uv() - p2->uv();
    auto b = p1->uv() - p3->uv();
    return a.u * b.v - a.v * b.u;
}

Vector
normal_triangle(const BDS_Point * p1, const BDS_Point * p2, const BDS_Point * p3)
{
    return vector_triangle(p1, p2, p3).normalized();
}

double
_cos_N(const BDS_Point * p1, const BDS_Point * p2, const BDS_Point * p3, const GeomSurface * gf)
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
    auto uv = 1. / 3. * (p1->uv() + p2->uv() + p3->uv());
    auto N = gf->normal(uv);
#endif
    return dot_product(N, n);
}

bool
is_equivalent(std::array<const BDS_Edge *, 3> e, std::array<const BDS_Edge *, 3> o)
{
    return (o[0] == e[0] && o[1] == e[1] && o[2] == e[2]) ||
           (o[0] == e[0] && o[1] == e[2] && o[2] == e[1]) ||
           (o[0] == e[1] && o[1] == e[0] && o[2] == e[2]) ||
           (o[0] == e[1] && o[1] == e[2] && o[2] == e[0]) ||
           (o[0] == e[2] && o[1] == e[0] && o[2] == e[1]) ||
           (o[0] == e[2] && o[1] == e[1] && o[2] == e[0]);
}

double
surface_triangle_param(const BDS_Point * p1, const BDS_Point * p2, const BDS_Point * p3)
{
    // FIXME
    // THIS ASSUMES DEGENERATED EDGES ALONG AXIS U !!!
    // SEEMS TO BE THE CASE WITH OCC

    if (!p1 || !p2 || !p3) {
        Log::error("Invalid point in parametric triangle surface computation");
        return 0;
    }

    double c;
    if ((p1->degenerated() ? 1 : 0) + (p2->degenerated() ? 1 : 0) + (p3->degenerated() ? 1 : 0) > 1)
        c = 0; // vector_triangle_parametric(p1, p2, p3, c);
    else if (p1->degenerated() == 1) {
        auto du = std::abs(p3->u() - p2->u());
        c = 2 * std::abs(0.5 * (p3->v() + p2->v()) - p1->v()) * du;
    }
    else if (p2->degenerated() == 1) {
        auto du = std::abs(p3->u() - p1->u());
        c = 2 * std::abs(0.5 * (p3->v() + p1->v()) - p2->v()) * du;
    }
    else if (p3->degenerated() == 1) {
        auto du = std::abs(p2->u() - p1->u());
        c = 2 * std::abs(0.5 * (p2->v() + p1->v()) - p3->v()) * du;
    }
    else if (p1->degenerated() == 2) {
        auto dv = std::abs(p3->v() - p2->v());
        c = 2 * std::abs(0.5 * (p3->u() + p2->u()) - p1->u()) * dv;
    }
    else if (p2->degenerated() == 2) {
        auto dv = std::abs(p3->v() - p1->v());
        c = 2 * std::abs(0.5 * (p3->u() + p1->u()) - p2->u()) * dv;
    }
    else if (p3->degenerated() == 2) {
        auto dv = std::abs(p2->v() - p1->v());
        c = 2 * std::abs(0.5 * (p2->u() + p1->u()) - p3->u()) * dv;
    }
    else
        c = vector_triangle_parametric(p1, p2, p3);
    return (0.5 * c);
}

bool
intersect_edges_2d(double x1,
                   double y1,
                   double x2,
                   double y2,
                   double x3,
                   double y3,
                   double x4,
                   double y4)
{
    std::array<std::array<double, 2>, 2> mat;
    std::array<double, 2> rhs;
    mat[0][0] = (x2 - x1);
    mat[0][1] = -(x4 - x3);
    mat[1][0] = (y2 - y1);
    mat[1][1] = -(y4 - y3);
    rhs[0] = x3 - x1;
    rhs[1] = y3 - y1;
    auto res = sys2x2(mat, rhs);
    if (not res.has_value())
        return false;
    auto x = res.value();
    if (x[0] >= 0.0 && x[0] <= 1.0 && x[1] >= 0.0 && x[1] <= 1.0)
        return true;
    return false;
}

bool
intersect_edges_2d(UVParam p1, UVParam p2, UVParam q1, UVParam q2)
{
    return intersect_edges_2d(p1.u, p1.v, p2.u, p2.v, q1.u, q1.v, q2.u, q2.v);
}

std::array<BDS_Point *, 2>
edge_opposite_to_vertex(const std::array<BDS_Point *, 3> & pts, const BDS_Point * p)
{
    if (pts[0] == p)
        return { pts[1], pts[2] };
    else if (pts[1] == p)
        return { pts[0], pts[2] };
    else
        return { pts[0], pts[1] };
}

Optional<std::vector<BDS_Point *>>
get_ordered_neighboring_vertices(const BDS_Point * p, const std::vector<BDS_Face *> & triangles)
{
    if (triangles.empty())
        return std::nullopt;

    std::vector<BDS_Point *> nbg;
    while (true) {
        bool found = false;
        for (auto & tri : triangles) {
            auto pts_res = tri->get_nodes();
            if (not pts_res.has_value())
                continue;
            auto pts = pts_res.value();
            auto pp = edge_opposite_to_vertex(pts, p);

            if (nbg.empty()) {
                nbg.push_back(pp[0]);
                nbg.push_back(pp[1]);
                found = true;
                break;
            }
            else {
                BDS_Point * p0 = nbg[nbg.size() - 2];
                BDS_Point * p1 = nbg[nbg.size() - 1];
                if (p1 == pp[0] && p0 != pp[1]) {
                    nbg.push_back(pp[1]);
                    found = true;
                    break;
                }
                else if (p1 == pp[1] && p0 != pp[0]) {
                    nbg.push_back(pp[0]);
                    found = true;
                    break;
                }
            }
        }

        if (nbg.size() == triangles.size())
            break;
        if (!found)
            return std::nullopt;
    }
    return nbg;
}

bool
validity_of_cavity(UVParam p, const std::vector<BDS_Point *> & nbg)
{
    UVParam q = { nbg[0]->degenerated() == 1 ? nbg[1]->u() : nbg[0]->u(),
                  nbg[0]->degenerated() == 2 ? nbg[1]->v() : nbg[0]->v() };
    UVParam r = { nbg[1]->degenerated() == 1 ? nbg[0]->u() : nbg[1]->u(),
                  nbg[1]->degenerated() == 2 ? nbg[0]->v() : nbg[1]->v() };
    auto sign = orient2d(p, q, r);
    for (size_t i = 1; i < nbg.size(); ++i) {
        auto * p0 = nbg[i];
        auto * p1 = nbg[(i + 1) % nbg.size()];
        UVParam qq = { p0->degenerated() == 1 ? p1->u() : p0->u(),
                       p0->degenerated() == 2 ? p1->v() : p0->v() };
        UVParam rr = { p1->degenerated() == 1 ? p0->u() : p1->u(),
                       p1->degenerated() == 2 ? p0->v() : p1->v() };
        auto s_sign = orient2d(p, qq, rr);
        if (sign * s_sign <= 0)
            return false;
    }
    return true;
}

std::tuple<double, double>
tutte_energy(Point pt, const std::vector<BDS_Point *> & nbg)
{
    if (nbg.empty())
        return { MAX_LC, 0. };
    double E = 0;
    double maximum = 0., minimum = 0.;
    for (size_t i = 0; i < nbg.size(); ++i) {
        const auto delta = (pt - nbg[i]->point());
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
centroid_uv(const BDS_Point * p,
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
        auto delta_uv = p->uv() - kernel[i];
        auto denom = dot_product(delta_uv, delta_uv);
        if (denom) {
            auto delta = p->point() - gp;
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

bool
minimize_tutte_energy_proj(const BDS_Point * p,
                           double E_unmoved,
                           const std::vector<BDS_Point *> & nbg,
                           const std::vector<UVParam> & kernel,
                           const std::vector<double> & lc,
                           const GeomSurface & gf)
{
    Point x;
    double sum = 0.;
    auto p0 = p->point();
    for (std::size_t i = 0; i < nbg.size(); ++i) {
        auto pi = nbg[i]->point();
        auto pip = nbg[(i + 1) % nbg.size()]->point();
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
    if (validity_of_cavity(uv, nbg)) {
        auto [E_moved, _] = tutte_energy(gp, nbg);
        if (E_moved < E_unmoved)
            return true;
    }
    return false;
}

bool
minimize_tutte_energy_param(BDS_Point * p,
                            double E_unmoved,
                            const std::vector<BDS_Point *> & nbg,
                            const std::vector<UVParam> & kernel,
                            const std::vector<double> & lcs,
                            const GeomSurface & gf)
{
    auto [uv, LC] = centroid_uv(p, gf, kernel, lcs);
    auto gp = gf.point(uv);
    auto [E_moved, ratio2] = tutte_energy(gp, nbg);
    if (E_moved < E_unmoved) {
        if (!validity_of_cavity(uv, nbg))
            return false;
        p->set_lc(LC);
        return ratio2 > .25;
    }
    return false;
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

std::tuple<std::vector<UVParam>, std::vector<double>>
compute_some_kind_of_kernel(const BDS_Point * p, const std::vector<BDS_Point *> & nbg)
{
    std::vector<UVParam> kernels;
    std::vector<double> lcs;

    auto pp = p->uv();
    auto ll = p->lc();
    for (std::size_t i = 0; i < nbg.size(); i++) {
        if (nbg[i]->degenerated() == 1) {
            kernels.push_back({ p->u(), nbg[i]->v() });
            kernels.push_back({ nbg[(i + 1) % nbg.size()]->u(), nbg[i]->v() });

            lcs.push_back(nbg[i]->lc());
            lcs.push_back(nbg[i]->lc());
        }
        else if (nbg[i]->degenerated() == 2) {
            kernels.push_back({ nbg[i]->u(), p->v() });
            kernels.push_back({ nbg[i]->u(), nbg[(i + 1) % nbg.size()]->v() });

            lcs.push_back(nbg[i]->lc());
            lcs.push_back(nbg[i]->lc());
        }
        else if (nbg[(i + 1) % nbg.size()]->degenerated() == 1) {
            kernels.push_back({ nbg[i]->u(), nbg[i]->v() });
            kernels.push_back({ nbg[i]->u(), nbg[(i + 1) % nbg.size()]->v() });
            lcs.push_back(nbg[i]->lc());
            lcs.push_back(nbg[i]->lc());
        }
        else if (nbg[(i + 1) % nbg.size()]->degenerated() == 2) {
            kernels.push_back({ nbg[i]->u(), nbg[i]->v() });
            kernels.push_back({ nbg[(i + 1) % nbg.size()]->u(), nbg[i]->v() });
            lcs.push_back(nbg[i]->lc());
            lcs.push_back(nbg[i]->lc());
        }
        else {
            kernels.push_back({ nbg[i]->u(), nbg[i]->v() });
            lcs.push_back(nbg[i]->lc());
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

} // namespace

BDS_GeomEntity::BDS_GeomEntity(int tag, int degree) : tag_(tag), degree_(degree) {}

BDS_GeomEntity::~BDS_GeomEntity() {}

int
BDS_GeomEntity::degree() const
{
    return this->degree_;
}

int
BDS_GeomEntity::tag() const
{
    return this->tag_;
}

bool
BDS_GeomEntity::operator<(const BDS_GeomEntity & other) const
{
    if (this->degree_ < other.degree_)
        return true;
    if (this->degree_ > other.degree_)
        return false;
    if (this->tag_ < other.tag_)
        return true;
    return false;
}

bool
BDS_GeomEntity::operator==(const BDS_GeomEntity & other) const
{
    return this->degree_ == other.degree_ && this->tag_ == other.tag_;
}

// BDS_Point

BDS_Point::BDS_Point(int id, Point pt, UVParam uv) :
    lc_pts_(MAX_LC),
    pt_(pt),
    uv_(uv),
    config_modified_(true),
    degenerated_(0),
    id_(id),
    periodic_counterpart_(nullptr),
    g_(nullptr)
{
}

i32
BDS_Point::id() const
{
    return this->id_;
}

double
BDS_Point::lc() const
{
    return this->lc_pts_;
}

void
BDS_Point::set_lc(double lc)
{
    this->lc_pts_ = lc;
}

Point
BDS_Point::point() const
{
    return this->pt_;
}

UVParam
BDS_Point::uv() const
{
    return this->uv_;
}

double
BDS_Point::u() const
{
    return this->uv_.u;
}

double
BDS_Point::v() const
{
    return this->uv_.v;
}

u8
BDS_Point::degenerated() const
{
    return this->degenerated_;
}

void
BDS_Point::del(BDS_Edge * e)
{
    if (this->edges_.empty())
        return;
    this->edges_.erase(std::remove(this->edges_.begin(), this->edges_.end(), e),
                       this->edges_.end());
}

std::vector<BDS_Face *>
BDS_Point::triangles() const
{
    std::vector<BDS_Face *> t;
    t.reserve(this->edges_.size());

    for (const auto & edge : this->edges_) {
        for (const auto & tt : edge->faces()) {
            if (tt && std::find(t.begin(), t.end(), tt) == t.end()) {
                t.push_back(tt);
            }
        }
    }
    return t;
}

bool
BDS_Point::operator<(const BDS_Point & other) const
{
    return this->id_ < other.id_;
}

// BDS_Edge

BDS_Edge::BDS_Edge(BDS_Point * a, BDS_Point * b) : deleted_(false), g_(nullptr)
{
    if (*a < *b) {
        this->p1_ = a;
        this->p2_ = b;
    }
    else {
        this->p1_ = b;
        this->p2_ = a;
    }
    this->p1_->edges_.push_back(this);
    this->p2_->edges_.push_back(this);
}

std::vector<BDS_Face *>
BDS_Edge::faces()
{
    return this->faces_;
}

double
BDS_Edge::length() const
{
    auto delta = this->p1_->point() - this->p2_->point();
    return delta.magnitude();
}

bool
BDS_Edge::deleted() const
{
    return this->deleted_;
}

bool
BDS_Edge::active() const
{
    return not this->deleted_;
}

void
BDS_Edge::del()
{
    this->p1_->del(this);
    this->p2_->del(this);
    this->deleted_ = true;
}

int
BDS_Edge::num_faces() const
{
    return static_cast<int>(this->faces_.size());
}

int
BDS_Edge::num_triangles() const
{
    return this->faces_.size();
}

BDS_Point *
BDS_Edge::common_vertex(const BDS_Edge * other) const
{
    if (this->p1_ == other->p1_ || this->p1_ == other->p2_)
        return this->p1_;
    if (this->p2_ == other->p1_ || this->p2_ == other->p2_)
        return this->p2_;
    Log::error("Edge {}-{} has no common node with edge {}-{}",
               this->p1_->id(),
               this->p2_->id(),
               other->p1_->id(),
               other->p2_->id());
    return nullptr;
}

BDS_Point *
BDS_Edge::other_vertex(const BDS_Point * p) const
{
    if (this->p1_ == p)
        return this->p2_;
    if (this->p2_ == p)
        return this->p1_;
    Log::error("Edge {}-{} does not contain node {}", this->p1_->id(), this->p2_->id(), p->id());
    return nullptr;
}

void
BDS_Edge::add_face(BDS_Face * f)
{
    this->faces_.push_back(f);
}

bool
BDS_Edge::operator<(const BDS_Edge & other) const
{
    if (*other.p1_ < *this->p1_)
        return true;
    if (*this->p1_ < *other.p1_)
        return false;
    if (*other.p2_ < *this->p2_)
        return true;
    return false;
}

Optional<BDS_Face *>
BDS_Edge::other_face(const BDS_Face * f) const
{
    if (num_faces() != 2) {
        Log::error("{} face(s) attached to edge {}-{}",
                   num_faces(),
                   this->p1_->id(),
                   this->p2_->id());
        return std::nullopt;
    }
    if (f == this->faces_[0])
        return this->faces_[1];
    if (f == this->faces_[1])
        return this->faces_[0];
    Log::error("Edge {}-{} does not belong to the face", this->p1_->id(), this->p2_->id());
    return std::nullopt;
}

void
BDS_Edge::del(BDS_Face * t)
{
    if (this->faces_.empty())
        return;
    // clang-format off
    this->faces_.erase(
        std::remove_if(
            this->faces_.begin(), this->faces_.end(),
            [t](BDS_Face * ptr) {
                return ptr == t;
            }
        ),
        this->faces_.end()
    );
    // clang-format on
}

BDS_Point *
BDS_Edge::opposite_vertex(const std::array<BDS_Point *, 3> & pts) const
{
    if (pts[0] != this->p1_ && pts[0] != this->p2_)
        return pts[0];
    else if (pts[1] != this->p1_ && pts[1] != this->p2_)
        return pts[1];
    else
        return pts[2];
}

std::array<BDS_Point *, 2>
BDS_Edge::opposite_of() const
{
    std::array<BDS_Point *, 2> oface = { nullptr, nullptr };
    if (this->faces_[0]) {
        auto pts_res = this->faces_[0]->get_nodes();
        if (not pts_res.has_value())
            return { nullptr, nullptr };
        oface[0] = opposite_vertex(pts_res.value());
    }
    if (this->faces_[1]) {
        auto pts_res = this->faces_[1]->get_nodes();
        if (not pts_res.has_value())
            return { nullptr, nullptr };
        oface[1] = opposite_vertex(pts_res.value());
    }
    return oface;
}

std::tuple<std::array<BDS_Point *, 3>, std::array<BDS_Point *, 3>, std::array<BDS_Point *, 2>>
BDS_Edge::compute_neighborhood() const
{
    std::array<BDS_Point *, 2> oface = { nullptr, nullptr };
    std::array<BDS_Point *, 3> pts1 = { nullptr, nullptr, nullptr };
    std::array<BDS_Point *, 3> pts2 = { nullptr, nullptr, nullptr };
    if (this->faces_[0]) {
        auto pts_res = this->faces_[0]->get_nodes();
        if (not pts_res.has_value())
            return { pts1, pts2, oface };
        pts1 = pts_res.value();
        oface[0] = opposite_vertex(pts1);
    }
    if (this->faces_[1]) {
        auto pts_res = this->faces_[1]->get_nodes();
        if (not pts_res.has_value())
            return { pts1, pts2, oface };
        pts2 = pts_res.value();
        oface[1] = opposite_vertex(pts2);
    }
    return { pts1, pts2, oface };
}

//

BDS_Face::BDS_Face(BDS_Edge * A, BDS_Edge * B, BDS_Edge * C) :
    deleted_(false),
    e1_(A),
    e2_(B),
    e3_(C),
    g_(nullptr)
{
    this->e1_->add_face(this);
    this->e2_->add_face(this);
    this->e3_->add_face(this);
}

bool
BDS_Face::deleted() const
{
    return this->deleted_;
}

bool
BDS_Face::active() const
{
    return not this->deleted_;
}

int
BDS_Face::num_edges() const
{
    return 3;
}

Optional<BDS_Edge *>
BDS_Face::opposite_edge(BDS_Point * p)
{
    if (this->e1_->p1_ != p && this->e1_->p2_ != p)
        return this->e1_;
    if (this->e2_->p1_ != p && this->e2_->p2_ != p)
        return this->e2_;
    if (this->e3_->p1_ != p && this->e3_->p2_ != p)
        return this->e3_;
    Log::error("Point {} does not belong to this triangle", p->id());
    return std::nullopt;
}

Optional<BDS_Point *>
BDS_Face::opposite_vertex(BDS_Edge * e)
{
    if (e == this->e1_)
        return this->e2_->common_vertex(this->e3_);
    if (e == this->e2_)
        return this->e1_->common_vertex(this->e3_);
    if (e == this->e3_)
        return this->e1_->common_vertex(this->e2_);
    Log::error("Edge {} {} does not belong to this triangle", e->p1_->id(), e->p2_->id());
    return std::nullopt;
}

Optional<std::array<BDS_Point *, 3>>
BDS_Face::get_nodes() const
{
    std::array<BDS_Point *, 3> n = { this->e1_->common_vertex(this->e3_),
                                     this->e1_->common_vertex(this->e2_),
                                     this->e2_->common_vertex(this->e3_) };
    if (n[0] && n[1] && n[2])
        return n;
    Log::error("Invalid points in face");
    return std::nullopt;
}

//

EdgeToRecover::EdgeToRecover(int p1, int p2, const GeomCurve * ge) : ge_(ge)
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

BDS_Mesh::BDS_Mesh(int max_pts) : max_point_num_(max_pts) {}

const std::map<int, Qtr<BDS_Point>> &
BDS_Mesh::points() const
{
    return this->points_;
}

Span<const Qtr<BDS_Edge>>
BDS_Mesh::edges() const
{
    return this->edges_;
}

Span<const Qtr<BDS_Face>>
BDS_Mesh::triangles() const
{
    return this->triangles_;
}

BDS_Point *
BDS_Mesh::add_point(int num, Point pt)
{
    auto pp = Qtr<BDS_Point>::alloc(num, pt, UVParam(0, 0));
    auto o = pp.get();
    this->points_.emplace(num, std::move(pp));
    this->max_point_num_ = std::max(this->max_point_num_, num);
    return o;
}

BDS_Point *
BDS_Mesh::add_point(int num, UVParam uv, const GeomSurface * gf)
{
    auto gp = gf->point(uv);
    auto pp = Qtr<BDS_Point>::alloc(num, gp, uv);
    auto o = pp.get();
    this->points_.emplace(num, std::move(pp));
    this->max_point_num_ = std::max(this->max_point_num_, num);
    return o;
}

void
BDS_Mesh::del_point(BDS_Point * p)
{
    if (!p)
        return;
    this->points_.erase(p->id());
}

Optional<BDS_Point *>
BDS_Mesh::find_point(int idx) const
{
    auto it = this->points_.find(idx);
    if (it != this->points_.end())
        return it->second.get();
    else
        return std::nullopt;
}

Optional<BDS_Edge *>
BDS_Mesh::add_edge(int idx1, int idx2)
{
    auto efound = find_edge(idx1, idx2);
    if (efound.has_value())
        return efound;

    auto pp1 = find_point(idx1);
    auto pp2 = find_point(idx2);

    if (not pp1.has_value() || not pp2.has_value()) {
        Log::error("Could not find points {} or {}", idx1, idx2);
        return std::nullopt;
    }

    auto edge = Qtr<BDS_Edge>::alloc(pp1.value(), pp2.value());
    auto ptr = edge.get();
    this->edges_.emplace_back(std::move(edge));
    return ptr;
}

void
BDS_Mesh::del_edge(BDS_Edge * e)
{
    if (!e)
        return;
    e->del();
}

Optional<BDS_Edge *>
BDS_Mesh::find_edge(int idx1, int idx2) const
{
    auto p_res = find_point(idx1);
    assert(p_res.has_value());
    auto p = p_res.value();
    return find_edge(p, idx2);
}

Optional<BDS_Edge *>
BDS_Mesh::find_edge(BDS_Point * p1, BDS_Point * p2) const
{
    return find_edge(p1, p2->id());
}

Optional<BDS_Edge *>
BDS_Mesh::find_edge(BDS_Point * p1, int p2) const
{
    for (auto & edge : p1->edges_) {
        if (edge->p1_ == p1 && edge->p2_->id() == p2)
            return edge;
        if (edge->p2_ == p1 && edge->p1_->id() == p2)
            return edge;
    }
    return std::nullopt;
}

Optional<BDS_Edge *>
BDS_Mesh::find_edge(BDS_Point * p1, BDS_Point * p2, BDS_Face * t) const
{
    BDS_Point P1(p1->id(), Point(), UVParam());
    BDS_Point P2(p2->id(), Point(), UVParam());
    BDS_Edge E(&P1, &P2);
    if (t->e1_->p1_->id() == E.p1_->id() && t->e1_->p2_->id() == E.p2_->id())
        return t->e1_;
    if (t->e2_->p1_->id() == E.p1_->id() && t->e2_->p2_->id() == E.p2_->id())
        return t->e2_;
    if (t->e3_->p1_->id() == E.p1_->id() && t->e3_->p2_->id() == E.p2_->id())
        return t->e3_;
    return std::nullopt;
}

Optional<BDS_Face *>
BDS_Mesh::add_triangle(int idx1, int idx2, int idx3)
{
    auto e1 = add_edge(idx1, idx2);
    auto e2 = add_edge(idx2, idx3);
    auto e3 = add_edge(idx3, idx1);
    if (e1.has_value() && e2.has_value() && e3.has_value())
        return add_triangle(e1.value(), e2.value(), e3.value());
    return std::nullopt;
}

Optional<BDS_Face *>
BDS_Mesh::add_triangle(BDS_Edge * e1, BDS_Edge * e2, BDS_Edge * e3)
{
    if (e1 && e2 && e3) {
        this->triangles_.emplace_back(Qtr<BDS_Face>::alloc(e1, e2, e3));
        return this->triangles_.back().get();
    }
    return std::nullopt;
}

void
BDS_Mesh::del_face(BDS_Face * t)
{
    if (!t)
        return;
    t->e1_->del(t);
    t->e2_->del(t);
    t->e3_->del(t);
    t->deleted_ = true;
}

Optional<BDS_Face *>
BDS_Mesh::find_triangle(BDS_Edge * e1, BDS_Edge * e2, BDS_Edge * e3) const
{
    for (const auto & t : e1->faces()) {
        if (is_equivalent({ e1, e2, e3 }, { t->e1_, t->e2_, t->e3_ })) {
            return t;
        }
    }
    for (const auto & t : e2->faces()) {
        if (is_equivalent({ e1, e2, e3 }, { t->e1_, t->e2_, t->e3_ })) {
            return t;
        }
    }
    for (const auto & t : e3->faces()) {
        if (is_equivalent({ e1, e2, e3 }, { t->e1_, t->e2_, t->e3_ })) {
            return t;
        }
    }
    return std::nullopt;
}

void
BDS_Mesh::add_geom(int tag, int degree)
{
    auto e = Qtr<BDS_GeomEntity>::alloc(tag, degree);
    this->geom_.emplace(std::move(e));
}

Optional<BDS_GeomEntity *>
BDS_Mesh::get_geom(int degree, int tag) const
{
    BDS_GeomEntity ge(degree, tag);
    // clang-format off
    auto it = std::find_if(
        this->geom_.begin(), this->geom_.end(),
        [&](const auto & ptr) {
            return *ptr == ge;
        }
    );
    // clang-format on
    if (it == this->geom_.end())
        return std::nullopt;
    return it->get();
}

Optional<BDS_Edge *>
BDS_Mesh::recover_edge(int num1,
                       int num2,
                       bool & fatal,
                       std::set<EdgeToRecover> * e2r,
                       std::set<EdgeToRecover> * not_recovered)
{
    auto e = find_edge(num1, num2);
    fatal = false;

    if (e.has_value())
        return e;

    auto p1_res = find_point(num1);
    auto p2_res = find_point(num2);

    if (not p1_res.has_value() || not p2_res.has_value()) {
        Log::error("Could not find points {} or {} in BDS mesh", num1, num2);
        return std::nullopt;
    }

    auto p1 = p1_res.value();
    auto p2 = p2_res.value();

    Log::debug("Edge {} {} has to be recovered", num1, num2);

    int ix = 0;
    while (true) {
        std::vector<BDS_Edge *> intersected;

        bool self_intersection = false;

        for (auto & e : this->edges_) {
            if (!e->deleted() && e->p1_ != p1 && e->p1_ != p2 && e->p2_ != p1 && e->p2_ != p2)
                if (intersect_edges_2d(e->p1_->uv(), e->p2_->uv(), p1->uv(), p2->uv())) {
                    // intersect
                    if (e2r && e2r->find(EdgeToRecover(e->p1_->id(), e->p2_->id(), nullptr)) !=
                                   e2r->end()) {
                        auto itr1 = e2r->find(EdgeToRecover(e->p1_->id(), e->p2_->id(), nullptr));
                        auto itr2 = e2r->find(EdgeToRecover(num1, num2, nullptr));
                        // Msg::Debug("edge %d %d on model edge %d cannot be recovered because"
                        //            " it intersects %d %d on model edge %d",
                        //            num1,
                        //            num2,
                        //            itr2->ge->tag(),
                        //            e->p1->iD,
                        //            e->p2->iD,
                        //            itr1->ge->tag());
                        // now throw a class that contains the diagnostic
                        not_recovered->insert(EdgeToRecover(num1, num2, itr2->geom_curve()));
                        not_recovered->insert(
                            EdgeToRecover(e->p1_->id(), e->p2_->id(), itr1->geom_curve()));
                        self_intersection = true;
                    }
                    if (e->num_faces() != e->num_triangles())
                        return std::nullopt;
                    intersected.push_back(e.get());
                }
        }

        if (self_intersection)
            return std::nullopt;

        if (!intersected.size() || ix > 300) {
            auto eee = find_edge(num1, num2);
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
            return eee;
        }

        std::vector<int>::size_type ichoice = 0;
        bool success = false;
        while (!success && ichoice < intersected.size()) {
            success = swap_edge(intersected[ichoice++], BDS_SwapEdgeTestRecover());
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

Optional<BDS_Edge *>
BDS_Mesh::recover_edge_fast(BDS_Point * p1, BDS_Point * p2)
{
    for (auto & tri : p1->triangles()) {
        auto edge = tri->opposite_edge(p1);
        // NOTE: promote the assert into runtime error
        assert(edge.has_value());
        auto face = edge.value()->other_face(tri);
        // NOTE: promote the assert into runtime error
        assert(face.has_value());
        auto p2b = face.value()->opposite_vertex(edge.value());
        if (p2b.has_value() && p2 == p2b.value()) {
            if (swap_edge(edge.value(), BDS_SwapEdgeTestRecover(), true))
                return find_edge(p1, p2->id());
        }
    }
    return std::nullopt;
}

bool
BDS_Mesh::swap_edge(BDS_Edge * e, const BDS_SwapEdgeTest & theTest, bool force)
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

    BDS_Point * p1 = e->p1_;
    BDS_Point * p2 = e->p2_;

    if (e->deleted_)
        return false;

    if (e->num_faces() != 2)
        return false;

    if (e->g_ && e->g_->degree() == 1)
        return false;

    auto [pts1, pts2, op] = e->compute_neighborhood();
    if (!op[0] || !op[1])
        return false;

    if (!force && !p1->config_modified_ && !p2->config_modified_ && !op[0]->config_modified_ &&
        !op[1]->config_modified_)
        return false;

    std::array<BDS_GeomEntity *, 2> g = { nullptr, nullptr };
    auto * ge = e->g_;

    // compute the orientation of the face
    // with respect to the edge
    int orientation = 0;
    for (int i = 0; i < 3; i++) {
        if (pts1[i] == p1) {
            orientation = pts1[(i + 1) % 3] == p2 ? 1 : -1;
            break;
        }
    }

    if (orientation == 1) {
        if (!theTest(p1, p2, op[0], p2, p1, op[1], p1, op[1], op[0], op[1], p2, op[0]))
            return false;
    }
    else {
        if (!theTest(p2, p1, op[0], p1, p2, op[1], p1, op[0], op[1], op[1], op[0], p2))
            return false;
    }

    if (!theTest(p1, p2, op[0], op[1]))
        return false;

    auto faces = e->faces();
    auto p1_op1 = find_edge(p1, op[0], faces[0]);
    auto op1_p2 = find_edge(op[0], p2, faces[0]);
    auto p1_op2 = find_edge(p1, op[1], faces[1]);
    auto op2_p2 = find_edge(op[1], p2, faces[1]);

    // degenerate
    if (p1_op1.value() == p1_op2.value() || op2_p2.value() == op1_p2.value())
        return false;

    for (auto i : { 0, 1 }) {
        if (faces[i]) {
            g[i] = faces[i]->g_;
            del_face(faces[i]);
        }
    }
    del_edge(e);

    auto edge = Qtr<BDS_Edge>::alloc(op[0], op[1]);
    auto edge_ptr = edge.get();
    this->edges_.emplace_back(std::move(edge));

    Qtr<BDS_Face> t1, t2;
    if (orientation == 1) {
        t1 = Qtr<BDS_Face>::alloc(p1_op1.value(), p1_op2.value(), edge_ptr);
        t2 = Qtr<BDS_Face>::alloc(edge_ptr, op2_p2.value(), op1_p2.value());
    }
    else {
        t1 = Qtr<BDS_Face>::alloc(p1_op2.value(), p1_op1.value(), edge_ptr);
        t2 = Qtr<BDS_Face>::alloc(op2_p2.value(), edge_ptr, op1_p2.value());
    }

    t1->g_ = g[0];
    t2->g_ = g[1];

    this->edges_.back()->g_ = ge;

    this->triangles_.emplace_back(std::move(t1));
    this->triangles_.emplace_back(std::move(t2));

    p1->config_modified_ = true;
    p2->config_modified_ = true;
    op[0]->config_modified_ = true;
    op[1]->config_modified_ = true;

    return true;
}

bool
BDS_Mesh::collapse_edge_parametric(BDS_Edge * e, BDS_Point * p, bool force)
{
    if (!force && e->num_faces() != 2)
        return false;
    if (!force && p->g_ && p->g_->degree() == 0)
        return false;
    // not really ok but 'til now this is the best choice not to do collapses on
    // model edges
    if (!force && p->g_ && p->g_->degree() == 1)
        return false;
    if (!force && e->g_ && p->g_) {
        if (e->g_->degree() == 2 && p->g_ != e->g_)
            return false;
    }

    if (!force) {
        for (std::size_t i = 0; i < e->p1_->edges_.size(); i++) {
            for (std::size_t j = 0; j < e->p2_->edges_.size(); j++) {
                BDS_Point * p1 = e->p1_->edges_[i]->p1_ == e->p1_ ? e->p1_->edges_[i]->p2_
                                                                  : e->p1_->edges_[i]->p1_;
                BDS_Point * p2 = e->p2_->edges_[j]->p1_ == e->p2_ ? e->p2_->edges_[j]->p2_
                                                                  : e->p2_->edges_[j]->p1_;
                if (p1->periodic_counterpart_ == p2)
                    return false;
            }
        }
    }

    if (e->num_faces() == 2) {
        auto oface = e->opposite_of();
        if (!oface[0] || !oface[1]) {
            Log::error("No opposite face in edge collapse");
            return false;
        }
        for (std::size_t i = 0; i < oface[0]->edges_.size(); i++) {
            if (oface[0]->edges_[i]->p1_ == oface[0] && oface[0]->edges_[i]->p2_ == oface[1])
                return false;
            if (oface[0]->edges_[i]->p1_ == oface[1] && oface[0]->edges_[i]->p2_ == oface[0])
                return false;
        }
        if (!force && oface[0]->g_ && oface[0]->g_->degree() == 2 && oface[0]->edges_.size() <= 4)
            return false;
        if (!force && oface[1]->g_ && oface[1]->g_->degree() == 2 && oface[1]->edges_.size() <= 4)
            return false;
        if (!force && oface[0]->g_ && oface[0]->g_->degree() < 2 && oface[0]->edges_.size() <= 3)
            return false;
        if (!force && oface[1]->g_ && oface[1]->g_->degree() < 2 && oface[1]->edges_.size() <= 3)
            return false;
    }
    auto tris = p->triangles();
    BDS_Point * o = e->other_vertex(p);

    BDS_Point * pt[3][1024];
    BDS_GeomEntity * gs[1024];
    int ept[2][1024];
    BDS_GeomEntity * egs[1024];
    int nt = 0;
    double area_old = 0.0;
    double area_new = 0.0;
    for (auto & t : tris) {
        auto pts_res = t->get_nodes();
        if (pts_res.has_value()) {
            auto pts = pts_res.value();
            double sold = std::abs(surface_triangle_param(pts[0], pts[1], pts[2]));
            area_old += sold;
            if (t->e1_ != e && t->e2_ != e && t->e3_ != e) {
                gs[nt] = t->g_;
                pt[0][nt] = (pts[0] == p) ? o : pts[0];
                pt[1][nt] = (pts[1] == p) ? o : pts[1];
                pt[2][nt] = (pts[2] == p) ? o : pts[2];
                if (!pt[0][nt] || !pt[1][nt] || !pt[2][nt]) {
                    return false;
                }
                double snew = std::abs(surface_triangle_param(pt[0][nt], pt[1][nt], pt[2][nt]));
                if (!force && snew < .02 * sold) {
                    return false;
                }
                area_new += snew;
                ++nt;
            }
        }
    }

    if (!force && std::abs(area_old - area_new) > 1.e-12 * (area_old + area_new)) {
        return false;
    }
    {
        auto it = tris.begin();
        while (it != tris.end()) {
            del_face(*it);
            ++it;
        }
    }

    int kk = 0;
    {
        std::vector<BDS_Edge *> edges(p->edges_);
        for (auto & edge : edges) {
            edge->p1_->config_modified_ = edge->p2_->config_modified_ = true;
            ept[0][kk] = (edge->p1_ == p) ? (o ? o->id() : -1) : edge->p1_->id();
            ept[1][kk] = (edge->p2_ == p) ? (o ? o->id() : -1) : edge->p2_->id();
            if (ept[0][kk] < 0 || ept[1][kk] < 0) {
                return false;
            }
            egs[kk++] = edge->g_;
            del_edge(edge);
        }
    }

    // FIXME
    // del_point(p);

    for (int k = 0; k < nt; k++) {
        auto t_res = add_triangle(pt[0][k]->id(), pt[1][k]->id(), pt[2][k]->id());
        assert(t_res.has_value());
        auto t = t_res.value();
        t->g_ = gs[k];
    }

    for (int i = 0; i < kk; ++i) {
        auto e = find_edge(ept[0][i], ept[1][i]);
        if (e.has_value() && not e.value()->g_)
            e.value()->g_ = egs[i];
    }

    return true;
}

bool
BDS_Mesh::smooth_point_centroid(BDS_Point * p, const GeomSurface & gf, double threshold)
{
    if (p->degenerated_)
        return false;
    if (p->g_ && p->g_->degree() <= 1)
        return false;
    if (p->g_ && p->g_->tag() < 0) {
        p->config_modified_ = true;
        return true;
    }

    auto res = get_ordered_neighboring_vertices(p, p->triangles());
    if (not res.has_value())
        return false;
    auto nbg = res.value();

    auto [E_unmoved, ratio] = tutte_energy(p->point(), nbg);
    if (ratio > threshold)
        return false;

    auto [kernels, lcs] = compute_some_kind_of_kernel(p, nbg);
    if (!minimize_tutte_energy_param(p, E_unmoved, nbg, kernels, lcs, gf)) {
        if (!minimize_tutte_energy_proj(p, E_unmoved, nbg, kernels, lcs, gf)) {
            return false;
        }
        else {
            p->config_modified_ = true;
            auto [E_unmoved, ratio] = tutte_energy(p->point(), nbg);
            minimize_tutte_energy_proj(p, E_unmoved, nbg, kernels, lcs, gf);
        }
    }
    else {
        p->config_modified_ = true;
    }

    return true;
}

bool
BDS_Mesh::split_edge(BDS_Edge * e, BDS_Point * mid, bool check_area_param)
{
    /*
          p1
        / | \
       /  |  \
   op1/ 0mid1 \op2
      \   |   /
       \  |  /
        \ p2/

       //  p1,op1,mid -
       //  p2,op2,mid -
       //  p2,op1,mid +
       //  p1,op2,mid +
    */

    BDS_Point * p1 = e->p1_;
    BDS_Point * p2 = e->p2_;

    auto op = e->opposite_of();
    if (!op[0] || !op[1])
        return false;

    if (check_area_param) {
        double area0 = std::abs(surface_triangle_param(p2, p1, op[0])) +
                       std::abs(surface_triangle_param(p2, p1, op[1]));
        double area1 = std::abs(surface_triangle_param(mid, p1, op[1])) +
                       std::abs(surface_triangle_param(mid, op[1], p2)) +
                       std::abs(surface_triangle_param(mid, p2, op[0])) +
                       std::abs(surface_triangle_param(mid, op[0], p1));
        // heuristic - abort if area changed too much
        if (area1 > 1.1 * area0 || area1 < 0.9 * area0) {
            return false;
        }
    }

    auto faces = e->faces();
    auto pts1_res = faces[0]->get_nodes();
    if (not pts1_res.has_value())
        return false;
    auto pts1 = pts1_res.value();

    int orientation = 0;
    for (int i = 0; i < 3; i++) {
        if (pts1[i] == p1) {
            orientation = pts1[(i + 1) % 3] == p2 ? 1 : -1;
            break;
        }
    }

    std::array<BDS_GeomEntity *, 2> g = { nullptr, nullptr };
    auto ge = e->g_;

    auto p1_op1 = find_edge(p1, op[0], faces[0]);
    auto op1_p2 = find_edge(op[0], p2, faces[0]);
    auto p1_op2 = find_edge(p1, op[1], faces[1]);
    auto op2_p2 = find_edge(op[1], p2, faces[1]);

    for (auto i : { 0, 1 }) {
        if (faces[i]) {
            g[i] = faces[i]->g_;
            del_face(faces[i]);
        }
    }
    del_edge(e);

    this->edges_.emplace_back(Qtr<BDS_Edge>::alloc(p1, mid));
    auto p1_mid = this->edges_.back().get();

    this->edges_.emplace_back(Qtr<BDS_Edge>::alloc(mid, p2));
    auto mid_p2 = this->edges_.back().get();

    this->edges_.emplace_back(Qtr<BDS_Edge>::alloc(op[0], mid));
    auto op1_mid = this->edges_.back().get();

    this->edges_.emplace_back(Qtr<BDS_Edge>::alloc(mid, op[1]));
    auto mid_op2 = this->edges_.back().get();

    Qtr<BDS_Face> t1, t2, t3, t4;
    if (orientation == 1) {
        t1 = Qtr<BDS_Face>::alloc(op1_mid, p1_op1.value(), p1_mid);
        t2 = Qtr<BDS_Face>::alloc(mid_op2, op2_p2.value(), mid_p2);
        t3 = Qtr<BDS_Face>::alloc(op1_p2.value(), op1_mid, mid_p2);
        t4 = Qtr<BDS_Face>::alloc(p1_op2.value(), mid_op2, p1_mid);
    }
    else {
        t1 = Qtr<BDS_Face>::alloc(p1_op1.value(), op1_mid, p1_mid);
        t2 = Qtr<BDS_Face>::alloc(op2_p2.value(), mid_op2, mid_p2);
        t3 = Qtr<BDS_Face>::alloc(op1_mid, op1_p2.value(), mid_p2);
        t4 = Qtr<BDS_Face>::alloc(mid_op2, p1_op2.value(), p1_mid);
    }
    t1->g_ = g[0];
    t2->g_ = g[1];
    t3->g_ = g[0];
    t4->g_ = g[1];

    p1_mid->g_ = ge;
    mid_p2->g_ = ge;
    op1_mid->g_ = g[0];
    mid_op2->g_ = g[1];

    mid->g_ = ge;

    this->triangles_.emplace_back(std::move(t1));
    this->triangles_.emplace_back(std::move(t2));
    this->triangles_.emplace_back(std::move(t3));
    this->triangles_.emplace_back(std::move(t4));

    return true;
}

void
BDS_Mesh::cleanup()
{
    {
        // clang-format off
        auto last = std::partition(
            this->triangles_.begin(), this->triangles_.end(),
            [](const auto & ptr) {
                return !ptr->deleted();
            }
        );
        // clang-format on
        this->triangles_.erase(last, this->triangles_.end());
    }
    {
        // clang-format off
        auto last = std::partition(
            this->edges_.begin(), this->edges_.end(),
            [](const auto & ptr) {
                return !ptr->deleted();
            }
        );
        // clang-format on
        this->edges_.erase(last, this->edges_.end());
    }
}

//

BDS_SwapEdgeTestRecover::BDS_SwapEdgeTestRecover() {}

bool
BDS_SwapEdgeTestRecover::operator()(const BDS_Point * p1,
                                    const BDS_Point * p2,
                                    const BDS_Point * q1,
                                    const BDS_Point * q2) const
{
    auto ori_t1 = orient2d(q1->uv(), p1->uv(), q2->uv());
    auto ori_t2 = orient2d(q1->uv(), q2->uv(), p2->uv());
    return (ori_t1 * ori_t2 > 0); // the quadrangle was strictly convex !
}

bool
BDS_SwapEdgeTestRecover::operator()(const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *,
                                    const BDS_Point *) const
{
    return true;
}

// This function does actually the swap without taking into account
// the feasability of the operation. Those conditions have to be
// taken into account before doing the edge swap

BDS_SwapEdgeTestQuality::BDS_SwapEdgeTestQuality(bool a, bool b) :
    test_quality_(a),
    test_small_triangles_(b)
{
}

bool
BDS_SwapEdgeTestQuality::operator()(const BDS_Point * p1,
                                    const BDS_Point * p2,
                                    const BDS_Point * q1,
                                    const BDS_Point * q2) const
{
    if (!this->test_small_triangles_)
        return true;

    // AVOID CREATING POINTS WITH 2 NEIGHBORING TRIANGLES
    //  std::vector<BDS_Face*> f1 = p1->getTriangles();
    //  std::vector<BDS_Face*> f2 = p2->getTriangles();
    if (p1->g_ && p1->g_->degree() == 2 && p1->edges_.size() <= 4)
        return false;
    if (p2->g_ && p2->g_->degree() == 2 && p2->edges_.size() <= 4)
        return false;
    if (p1->g_ && p1->g_->degree() < 2 && p1->edges_.size() <= 3)
        return false;
    if (p2->g_ && p2->g_->degree() < 2 && p2->edges_.size() <= 3)
        return false;

    auto s1 = std::abs(surface_triangle_param(p1, p2, q1));
    auto s2 = std::abs(surface_triangle_param(p1, p2, q2));
    auto s3 = std::abs(surface_triangle_param(p1, q1, q2));
    auto s4 = std::abs(surface_triangle_param(p2, q1, q2));
    if (std::abs(s1 + s2 - s3 - s4) > 1.e-12 * (s3 + s4))
        return false;
    else
        return true;
}

bool
BDS_SwapEdgeTestQuality::operator()(const BDS_Point * p1,
                                    const BDS_Point * p2,
                                    const BDS_Point * p3,
                                    const BDS_Point * q1,
                                    const BDS_Point * q2,
                                    const BDS_Point * q3,
                                    const BDS_Point * op1,
                                    const BDS_Point * op2,
                                    const BDS_Point * op3,
                                    const BDS_Point * oq1,
                                    const BDS_Point * oq2,
                                    const BDS_Point * oq3) const
{
    // Check if new edge is not on a seam or degenerated
    std::array<const BDS_Point *, 2> pts = { nullptr, nullptr };
    // const BDS_Point * p2 = nullptr;
    if (op1 != oq1 && op1 != oq2 && op1 != oq3) {
        pts = { op2, op3 };
    }
    else if (op2 != oq1 && op2 != oq2 && op2 != oq3) {
        pts = { op1, op3 };
    }
    else if (op3 != oq1 && op3 != oq2 && op3 != oq3) {
        pts = { op1, op2 };
    }
    else {
        Log::warn("Unable to detect the new edge in BDS_SwapEdgeTestQuality");
    }

    if (pts[0] && pts[1]) {
        if (pts[0]->degenerated() && pts[1]->degenerated())
            return false;
        if (pts[0]->periodic_counterpart_ && pts[1]->periodic_counterpart_)
            return false;
    }

    if (!this->test_quality_)
        return true;

    const auto qa1 = Tri3::gamma(p1->point(), p2->point(), p3->point());
    const auto qa2 = Tri3::gamma(q1->point(), q2->point(), q3->point());
    const auto qb1 = Tri3::gamma(op1->point(), op2->point(), op3->point());
    const auto qb2 = Tri3::gamma(oq1->point(), oq2->point(), oq3->point());

    // we swap for a better configuration
    const auto mina = std::min(qa1, qa2);
    const auto minb = std::min(qb1, qb2);

    return minb > mina;
}

BDS_SwapEdgeTestNormals::BDS_SwapEdgeTestNormals(GeomSurface * gf, double ori) : gf_(gf), ori_(ori)
{
}

bool
BDS_SwapEdgeTestNormals::operator()(const BDS_Point * p1,
                                    const BDS_Point * p2,
                                    const BDS_Point * q1,
                                    const BDS_Point * q2) const
{
    auto s1 = std::abs(surface_triangle_param(p1, p2, q1));
    auto s2 = std::abs(surface_triangle_param(p1, p2, q2));
    auto s3 = std::abs(surface_triangle_param(p1, q1, q2));
    auto s4 = std::abs(surface_triangle_param(p2, q1, q2));
    if (std::abs(s1 + s2 - s3 - s4) > 1.e-12 * (s3 + s4)) {
        return false;
    }
    return true;
}

bool
BDS_SwapEdgeTestNormals::operator()(const BDS_Point * p1,
                                    const BDS_Point * p2,
                                    const BDS_Point * p3,
                                    const BDS_Point * q1,
                                    const BDS_Point * q2,
                                    const BDS_Point * q3,
                                    const BDS_Point * op1,
                                    const BDS_Point * op2,
                                    const BDS_Point * op3,
                                    const BDS_Point * oq1,
                                    const BDS_Point * oq2,
                                    const BDS_Point * oq3) const
{
    auto qa1 = Tri3::gamma(p1->point(), p2->point(), p3->point());
    auto qa2 = Tri3::gamma(q1->point(), q2->point(), q3->point());
    auto qb1 = Tri3::gamma(op1->point(), op2->point(), op3->point());
    auto qb2 = Tri3::gamma(oq1->point(), oq2->point(), oq3->point());

    double OLD = std::min(this->ori_ * qa1 * _cos_N(p1, p2, p3, this->gf_),
                          this->ori_ * qa2 * _cos_N(q1, q2, q3, this->gf_));
    double NEW = std::min(this->ori_ * qb1 * _cos_N(op1, op2, op3, this->gf_),
                          this->ori_ * qb2 * _cos_N(oq1, oq2, oq3, this->gf_));

    if (OLD < 0.5 && OLD < NEW)
        return true;
    return false;
}

void
recur_tag(BDS_Face * t, BDS_GeomEntity * g)
{
    std::stack<BDS_Face *> stack;
    stack.push(t);

    while (!stack.empty()) {
        t = stack.top();
        stack.pop();
        if (!t->g_) {
            t->g_ = g;
            if (!t->e1_->g_ && t->e1_->num_faces() == 2) {
                stack.push(t->e1_->other_face(t).value());
            }
            if (!t->e2_->g_ && t->e2_->num_faces() == 2) {
                stack.push(t->e2_->other_face(t).value());
            }
            if (!t->e3_->g_ && t->e3_->num_faces() == 2) {
                stack.push(t->e3_->other_face(t).value());
            }
        }
    }
}

} // namespace krado
