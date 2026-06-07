// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/scheme/delaunay.h"
#include "krado/consts.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/geom_surface.h"
#include "krado/geom_curve.h"
#include "krado/bds.h"
#include "krado/log.h"
#include "krado/poly_mesh.h"
#include "krado/bounding_box_3d.h"
#include "krado/uv_param.h"
#include "krado/numerics.h"
#include "krado/utils.h"
#include "krado/element.h"
#include "krado/predicates.h"
#include "krado/metric.h"
#include "krado/exception.h"
#include "krado/element.h"
#include "krado/vector.h"
#include "krado/bidim_mesh_data.h"
#include "krado/predicates.h"
#include <Eigen/Dense>
#include <map>
#include <list>

namespace krado {

namespace {

const double LIMIT = 0.5 * std::sqrt(2.0) * 1;

struct Metric {
    Metric(double a, double b, double c) : val_({ a, b, c }) {}

private:
    std::array<double, 3> val_;

public:
    double
    operator[](int idx) const
    {
        return this->val_[idx];
    }

    /// Build metric
    ///
    /// @param surface Geometrical surface
    /// @param uv Parametrical point
    /// @return Computed metric
    static Metric
    build(const GeomSurface & surface, UVParam uv)
    {
        auto [du, dv] = surface.d1(uv);
        Metric metric(dot_product(du, du), dot_product(dv, du), dot_product(dv, dv));
        return metric;
    }
};

struct NodeCopies {
    // max 8 copies -- reduced to 4
    static constexpr std::size_t MAX_COPIES = 8;
    Ptr<MeshVertexAbstract> mv;
    std::size_t n_copies;
    std::array<UVParam, MAX_COPIES> uv;
    std::array<std::size_t, MAX_COPIES> id;

    NodeCopies(Ptr<MeshVertexAbstract> _mv, UVParam _uv) : mv(_mv), n_copies(1)
    {
        this->uv[0] = _uv;
        std::fill(this->id.begin(), this->id.end(), 0);
    }

    void
    add_copy(UVParam _uv)
    {
        for (std::size_t i = 0; i < this->n_copies; i++) {
            if (std::abs(this->uv[i].u - _uv.u) < 1.e-9 && std::abs(this->uv[i].v - _uv.v) < 1.e-9)
                return;
        }
        this->uv[this->n_copies] = _uv;
        this->n_copies++;
    }

    std::size_t
    closest(UVParam _uv)
    {
        double minD = MAX_LC;
        std::size_t I = 0;
        for (std::size_t i = 0; i < this->n_copies; i++) {
            auto dist = utils::distance(_uv, this->uv[i]);
            if (dist < minD) {
                minD = dist;
                I = i;
            }
        }
        return this->id[I];
    }
};

struct BidimMeshData {
    std::map<Ptr<MeshVertexAbstract>, int> indices;
    std::vector<UVParam> uv;
    std::vector<double> v_sizes;
    std::map<Ptr<MeshVertexAbstract>, Ptr<MeshVertexAbstract>> * equivalence;
    std::map<Ptr<MeshVertexAbstract>, UVParam> * parametric_coordinates;
    std::set<MeshElement, MeshElementLessThan> internal_edges; // embedded edges
    //  std::set<MVertex*> internalVertices; // embedded vertices

    BidimMeshData(std::map<Ptr<MeshVertexAbstract>, Ptr<MeshVertexAbstract>> * e = nullptr,
                  std::map<Ptr<MeshVertexAbstract>, UVParam> * p = nullptr) :
        equivalence(e),
        parametric_coordinates(p)
    {
    }

    void
    add_vertex(Ptr<MeshVertexAbstract> mv, UVParam uv, double size)
    {
        int index = this->uv.size();
        this->indices[mv] = index;
        if (this->parametric_coordinates) {
            auto it = this->parametric_coordinates->find(mv);
            if (it != this->parametric_coordinates->end()) {
                uv = it->second;
            }
        }
        this->uv.push_back(uv);
        this->v_sizes.push_back(size);
    }

    int
    index(Ptr<MeshVertexAbstract> mv) const
    {
        return this->indices.at(mv);
    }

    Ptr<MeshVertexAbstract>
    equivalent(Ptr<MeshVertexAbstract> v1) const
    {
        if (this->equivalence) {
            auto it = this->equivalence->find(v1);
            if (it == this->equivalence->end())
                return nullptr;
            return it->second;
        }
        return nullptr;
    }
};

UVParam
circ_uv(const MeshElement & t, BidimMeshData & data)
{
    auto index0 = data.index(t.vertex(0));
    auto index1 = data.index(t.vertex(1));
    auto index2 = data.index(t.vertex(2));
    auto ctr = Tri3::circum_center(data.uv[index0], data.uv[index1], data.uv[index2]);
    if (ctr.has_value())
        return ctr.value();
    else
        return { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() };
}

std::tuple<UVParam, bool>
invMapUV(const MeshElement & t, UVParam p, const BidimMeshData & data, double tol)
{
    std::array<std::array<double, 2>, 2> mat;
    std::array<double, 2> b;

    auto index0 = data.index(t.vertex(0));
    auto index1 = data.index(t.vertex(1));
    auto index2 = data.index(t.vertex(2));

    auto u0 = data.uv[index0].u;
    auto v0 = data.uv[index0].v;
    auto u1 = data.uv[index1].u;
    auto v1 = data.uv[index1].v;
    auto u2 = data.uv[index2].u;
    auto v2 = data.uv[index2].v;

    mat[0][0] = u1 - u0;
    mat[0][1] = u2 - u0;
    mat[1][0] = v1 - v0;
    mat[1][1] = v2 - v0;

    b[0] = p.u - u0;
    b[1] = p.v - v0;
    auto res = sys2x2(mat, b);
    if (res.has_value()) {
        auto uv = res.value();
        auto inside = uv[0] >= -tol && uv[1] >= -tol && uv[0] <= 1. + tol && uv[1] <= 1. + tol &&
                      1. - uv[0] - uv[1] > -tol;
        return { { uv[0], uv[1] }, inside };
    }
    else {
        return { { 0., 0. }, false };
    }
}

double
getSurfUV(const MeshElement & t, BidimMeshData & data)
{
    auto index0 = data.index(t.vertex(0));
    auto index1 = data.index(t.vertex(1));
    auto index2 = data.index(t.vertex(2));

    auto u1 = data.uv[index0].u;
    auto v1 = data.uv[index0].v;
    auto u2 = data.uv[index1].u;
    auto v2 = data.uv[index1].v;
    auto u3 = data.uv[index2].u;
    auto v3 = data.uv[index2].v;

    const double vv1[2] = { u2 - u1, v2 - v1 };
    const double vv2[2] = { u3 - u1, v3 - v1 };

    return 0.5 * (vv1[0] * vv2[1] - vv1[1] * vv2[0]);
}

double
compute_tolerance(double radius)
{
    if (radius <= 1e3)
        return 1e-12;
    if (radius <= 1e5)
        return 1e-11;
    return 1e-9;
}

std::tuple<UVParam, double>
circum_center_metric(UVParam pa, UVParam pb, UVParam pc, Metric metric)
{
    // d = (u2-u1) M (u2-u1) = u2 M u2 + u1 M u1 - 2 u2 M u1
    std::array<std::array<double, 2>, 2> sys;
    std::array<double, 2> rhs;

    const double a = metric[0];
    const double b = metric[1];
    const double d = metric[2];

    sys[0][0] = 2. * a * (pa.u - pb.u) + 2. * b * (pa.v - pb.v);
    sys[0][1] = 2. * d * (pa.v - pb.v) + 2. * b * (pa.u - pb.u);
    sys[1][0] = 2. * a * (pa.u - pc.u) + 2. * b * (pa.v - pc.v);
    sys[1][1] = 2. * d * (pa.v - pc.v) + 2. * b * (pa.u - pc.u);

    rhs[0] = a * (pa.u * pa.u - pb.u * pb.u) + d * (pa.v * pa.v - pb.v * pb.v) +
             2. * b * (pa.u * pa.v - pb.u * pb.v);
    rhs[1] = a * (pa.u * pa.u - pc.u * pc.u) + d * (pa.v * pa.v - pc.v * pc.v) +
             2. * b * (pa.u * pa.v - pc.u * pc.v);

    auto res = sys2x2(sys, rhs);
    std::array<double, 2> x = res.has_value() ? res.value() : std::array<double, 2> { 0., 0. };

    double radius = (x[0] - pa.u) * (x[0] - pa.u) * a + (x[1] - pa.v) * (x[1] - pa.v) * d +
                    2. * (x[0] - pa.u) * (x[1] - pa.v) * b;
    return { { x[0], x[1] }, radius };
}

std::tuple<UVParam, double>
circum_center_metric(const MeshElement & base, Metric metric, const BidimMeshData & data)
{
    // d = (u2-u1) M (u2-u1) = u2 M u2 + u1 M u1 - 2 u2 M u1
    auto index0 = data.index(base.vertex(0));
    auto index1 = data.index(base.vertex(1));
    auto index2 = data.index(base.vertex(2));
    return circum_center_metric(data.uv[index0], data.uv[index1], data.uv[index2], metric);
}

bool
in_circum_circle_aniso(UVParam p1, UVParam p2, UVParam p3, UVParam uv, Metric metric)
{
    auto [x, radius2] = circum_center_metric(p1, p2, p3, metric);
    const double a = metric[0];
    const double b = metric[1];
    const double d = metric[2];
    const double d0 = (x.u - uv.u);
    const double d1 = (x.v - uv.v);
    const double d3 = d0 * d0 * a + d1 * d1 * d + 2.0 * d0 * d1 * b;
    const auto tolerance = compute_tolerance(radius2);
    return d3 < radius2 - tolerance;
}

bool
inCircumCircleAniso(const MeshElement & base, UVParam uv, Metric metric, BidimMeshData & data)
{
    auto [x, radius2] = circum_center_metric(base, metric, data);
    const double a = metric[0];
    const double b = metric[1];
    const double d = metric[2];
    const double d0 = (x.u - uv.u);
    const double d1 = (x.v - uv.v);
    const double d3 = d0 * d0 * a + d1 * d1 * d + 2.0 * d0 * d1 * b;
    return d3 < radius2;
}

bool
intersection_segments_2(UVParam p1, UVParam p2, UVParam q1, UVParam q2)
{
    auto a = orient2d(p1, p2, q1);
    auto b = orient2d(p1, p2, q2);
    if (a * b > 0)
        return false;
    a = orient2d(q1, q2, p1);
    b = orient2d(q1, q2, p2);
    if (a * b > 0)
        return false;
    return true;
}

class Triangle {
private:
    bool deleted_;
    double circum_radius_;
    MeshElement base_;
    std::array<Triangle *, Tri3::N_EDGES> neigh_;

public:
    Triangle(const MeshElement & t, double circum_radius) :
        deleted_(false),
        circum_radius_(circum_radius),
        base_(t),
        neigh_({ nullptr, nullptr, nullptr })
    {
    }

    bool
    is_deleted() const
    {
        return this->deleted_;
    }

    void
    force_radius(double r)
    {
        this->circum_radius_ = r;
    }

    double
    radius() const
    {
        return this->circum_radius_;
    }

    Optional<Ptr<MeshVertexAbstract>>
    other_side(int i)
    {
        auto * n = this->neigh_[i];
        if (!n)
            return std::nullopt;
        auto v1 = this->base_.vertex((i + 2) % 3);
        auto v2 = this->base_.vertex(i);
        for (int j = 0; j < Tri3::N_VERTICES; j++)
            if (n->tri().vertex(j) != v1 && n->tri().vertex(j) != v2)
                return n->tri().vertex(j);
        return std::nullopt;
    }

    void
    set_tri(const MeshElement & t)
    {
        this->base_ = t;
    }

    const MeshElement &
    tri() const
    {
        return this->base_;
    }

    void
    set_neighbor(int idx, Triangle * n)
    {
        this->neigh_[idx] = n;
    }

    Triangle *
    neighbor(int idx) const
    {
        return this->neigh_[idx];
    }

    void
    set_deleted(bool flag)
    {
        this->deleted_ = flag;
    }

    bool
    assert_neigh() const
    {
        if (this->deleted_)
            return true;
        for (int i = 0; i < Tri3::N_EDGES; i++)
            if (this->neigh_[i] && (this->neigh_[i]->is_neigh(this) == false))
                return false;
        return true;
    }

    bool
    is_neigh(const Triangle * t) const
    {
        for (int i = 0; i < Tri3::N_EDGES; i++)
            if (this->neigh_[i] == t)
                return true;
        return false;
    }
};

class CompareTrianglePtr {
    MeshElementLessThan lf_;

public:
    bool
    operator()(const Triangle * a, const Triangle * b) const
    {
        if (a->radius() > b->radius())
            return true;
        if (a->radius() < b->radius())
            return false;
        // note: this effectively builds 3 triangles and compare them using the MeshElementLessThan
        // operator()
        return this->lf_(a->tri(), b->tri());
    }
};

struct edgeXface {
    std::array<Ptr<MeshVertexAbstract>, 2> v;
    Triangle * t1;
    int i1;
    int ori;

    edgeXface(Triangle * t, int i_fac) : t1(t), i1(i_fac), ori(1)
    {
        v[0] = t1->tri().vertex(i_fac == 0 ? 2 : i_fac - 1);
        v[1] = t1->tri().vertex(i_fac);
        if (v[0]->num() > v[1]->num()) {
            ori = -1;
            std::swap(v[0], v[1]);
        }
    }

    Ptr<MeshVertexAbstract>
    vertex(int i) const
    {
        return v[i];
    }

    bool
    operator<(const edgeXface & other) const
    {
        if (vertex(0)->num() < other.vertex(0)->num())
            return true;
        if (vertex(0)->num() > other.vertex(0)->num())
            return false;
        if (vertex(1)->num() < other.vertex(1)->num())
            return true;
        return false;
    }

    bool
    operator==(const edgeXface & other) const
    {
        if (vertex(0)->num() == other.vertex(0)->num() &&
            vertex(1)->num() == other.vertex(1)->num())
            return true;
        return false;
    }
};

void
setLcsInit(const MeshElement & t, std::map<Ptr<MeshVertexAbstract>, double> & v_sizes)
{
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
            auto vi = t.vertex(i);
            auto vj = t.vertex(j);
            v_sizes[vi] = -1;
            v_sizes[vj] = -1;
        }
    }
}

void
setLcs(const MeshElement & t,
       std::map<Ptr<MeshVertexAbstract>, double> & v_sizes,
       BidimMeshData & data)
{
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
            auto vi = t.vertex(i);
            auto vj = t.vertex(j);
            if (vi != data.equivalent(vj) && vj != data.equivalent(vi)) {
                auto l = utils::distance(vi->point(), vj->point());
                auto iti = v_sizes.find(vi);
                auto itj = v_sizes.find(vj);
                if (iti->second < 0 || iti->second > l)
                    iti->second = l;
                if (itj->second < 0 || itj->second > l)
                    itj->second = l;
            }
        }
    }
}

bool
isActive(Triangle * t, double limit, int & active)
{
    if (t->is_deleted())
        return false;
    for (active = 0; active < 3; active++) {
        auto * neigh = t->neighbor(active);
        if (!neigh || (neigh->radius() < limit && neigh->radius() > 0)) {
            return true;
        }
    }
    return false;
}

template <class Iterator>
void
connectTris(Iterator beg, Iterator end, std::vector<edgeXface> & conn)
{
    conn.clear();

    while (beg != end) {
        if (!(*beg)->is_deleted()) {
            for (int j = 0; j < 3; j++) {
                conn.push_back(edgeXface(*beg, j));
            }
        }
        ++beg;
    }

    if (conn.empty())
        return;

    std::sort(conn.begin(), conn.end());

    for (std::size_t i = 0; i < conn.size() - 1; i++) {
        auto & f1 = conn[i];
        auto & f2 = conn[i + 1];

        if (f1 == f2 && f1.t1 != f2.t1) {
            f1.t1->set_neighbor(f1.i1, f2.t1);
            f2.t1->set_neighbor(f2.i1, f1.t1);
            ++i;
        }
    }
}

#if 0

void
connectTriangles(std::list<Triangle *> & l)
{
    std::vector<edgeXface> conn;
    connectTris(l.begin(), l.end(), conn);
}

#endif

void
connectTriangles(std::set<Triangle *, CompareTrianglePtr> & l)
{
    std::vector<edgeXface> conn;
    connectTris(l.begin(), l.end(), conn);
}

#if 0

int
inCircumCircleXY(const MeshElement & t, Ptr<MeshVertexAbstract> v)
{
    auto v1 = t.vertex(0);
    auto v2 = t.vertex(1);
    auto v3 = t.vertex(2);
    auto loc1 = v1->point();
    auto loc2 = v2->point();
    auto loc3 = v3->point();
    UVParam p1 = { loc1.x, loc1.y };
    UVParam p2 = { loc2.x, loc2.y };
    UVParam p3 = { loc3.x, loc3.y };
    auto locpp = v->point();
    UVParam pp = { locpp.x, locpp.y };
    double result = incircle(p1, p2, p3, pp) * orient2d(p1, p2, p3);
    return (result > 0) ? 1 : 0;
}

void
recurFindCavity(std::vector<edgeXface> & shell,
                std::vector<Triangle *> & cavity,
                Ptr<MeshVertexAbstract> v,
                Triangle * t)
{
    t->set_deleted(true);
    // the cavity that has to be removed because it violates delaunay
    // criterion
    cavity.push_back(t);

    for (int i = 0; i < 3; i++) {
        auto * neigh = t->neighbor(i);
        if (!neigh)
            shell.push_back(edgeXface(t, i));
        else if (!neigh->is_deleted()) {
            int circ = inCircumCircleXY(neigh->tri(), v);
            if (circ)
                recurFindCavity(shell, cavity, v, neigh);
            else
                shell.push_back(edgeXface(t, i));
        }
    }
}

#endif

void
recurFindCavityAniso(Ptr<MeshSurface> surface,
                     std::list<edgeXface> & shell,
                     std::list<Triangle *> & cavity,
                     Metric metric,
                     UVParam param,
                     Triangle * t,
                     BidimMeshData & data)
{
    t->set_deleted(true);
    // the cavity that has to be removed because it violates delaunay
    // criterion
    cavity.push_back(t);

    for (int i = 0; i < Tri3::N_EDGES; i++) {
        auto * neigh = t->neighbor(i);
        edgeXface exf(t, i);
#if 0
        // take care of untouchable internal edges
        auto it = data.internal_edges.find(MeshElement::Line2({ exf.vertex(0), exf.vertex(1) }));
        if (neigh == nullptr || it != data.internal_edges.end())
            shell.push_back(exf);
        else
#endif
        if (neigh == nullptr)
            shell.push_back(exf);
        else if (not neigh->is_deleted()) {
            auto circ = inCircumCircleAniso(neigh->tri(), param, metric, data);
            if (circ)
                recurFindCavityAniso(surface, shell, cavity, metric, param, neigh, data);
            else
                shell.push_back(exf);
        }
    }
}

bool
buildMeshGenerationDataStructures(Ptr<MeshSurface> surface,
                                  std::set<Triangle *, CompareTrianglePtr> & all_tris,
                                  BidimMeshData & data)
{
    std::map<Ptr<MeshVertexAbstract>, double> v_sizes_map;

    for (auto & tri : surface->triangles())
        setLcsInit(tri, v_sizes_map);

    auto itfind = v_sizes_map.find(nullptr);
    if (itfind != v_sizes_map.end()) {
        Log::error("Some NULL points exist in 2D mesh");
        return false;
    }

    for (auto & tri : surface->triangles())
        setLcs(tri, v_sizes_map, data);

    // NOTE: just a rough sketch what should happen here - will need fixing
    // take care of embedded vertices
    for (auto & v : surface->embedded_vertices()) {
        v_sizes_map[v] = std::min(v_sizes_map[v], v->mesh_size());
    }

    // take care of embedded edges
    for (auto & crv : surface->embedded_curves()) {
        if (crv->is_mesh_degenerated())
            continue;

        for (auto & seg : crv->segments())
            data.internal_edges.insert(seg);
    }

    // take care of small edges in  order not to "pollute" the size field
    for (auto & crv : surface->curves()) {
        if (crv->is_mesh_degenerated())
            continue;

        for (auto & seg : crv->segments()) {
            auto v0 = seg.vertex(0);
            auto v1 = seg.vertex(1);

            double d = utils::distance(v0->point(), v1->point());
            double d0 = v_sizes_map[v0];
            double d1 = v_sizes_map[v1];
            if (d0 < .5 * d)
                v_sizes_map[v0] = .5 * d;
            if (d1 < .5 * d)
                v_sizes_map[v1] = .5 * d;
        }
    }

    auto & geom_surface = surface->geom_surface();
    for (auto & [vtx, size] : v_sizes_map) {
        auto param = reparam_mesh_vertex_on_surface(vtx, geom_surface);
        data.add_vertex(vtx, param, size);
    }
    for (auto & tri : surface->triangles()) {
        auto v0 = tri.vertex(0);
        auto v1 = tri.vertex(1);
        auto v2 = tri.vertex(2);

        double lc = (data.v_sizes[data.index(v0)] + data.v_sizes[data.index(v1)] +
                     data.v_sizes[data.index(v2)]) /
                    3.;

        auto circum_radius = circum_radius_euclidian(tri, lc);
        all_tris.insert(new Triangle(tri, circum_radius));
    }
    surface->remove_all_triangles();
    connectTriangles(all_tris);

    return true;
}

struct MeshVertexPtrLessThan {
    bool
    operator()(const Ptr<MeshVertexAbstract> & v1, const Ptr<MeshVertexAbstract> & v2) const
    {
        return v1->num() < v2->num();
    }
};

// build a set with all points of the boundaries
std::tuple<std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan>,
           std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan>>
build_vertices(Ptr<MeshSurface> surface, Span<const Ptr<MeshCurve>> curves)
{
    std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan> all_vertices;
    std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan> boundary;

    auto & geom_surface = surface->geom_surface();
    for (auto & crv : curves) {
        auto & geom_curve = crv->geom_curve();
        if (geom_curve.is_seam(geom_surface))
            return { {}, {} };

        if (not crv->is_mesh_degenerated()) {
            for (auto & seg : crv->segments()) {
                auto v1 = seg.vertex(0);
                auto v2 = seg.vertex(1);

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
            Log::debug("Degenerated mesh on edge {}", crv->id());
    }

    return { all_vertices, boundary };
}

/// construct set with embedded vertices from embedded edges and embedded vertices
std::set<Ptr<MeshVertexAbstract>>
build_embedded_vertices(Ptr<MeshSurface> surface)
{
    std::set<Ptr<MeshVertexAbstract>> vertices;
    // add embedded vertices
    for (auto & crv : surface->embedded_curves()) {
        if (crv->is_mesh_degenerated())
            continue;

        for (auto & vtx : crv->bounding_vertices())
            vertices.insert(vtx);
        for (auto & vtx : crv->curve_vertices())
            vertices.insert(vtx);
    }

    // add embedded vertices
    for (auto & vtx : surface->embedded_vertices())
        vertices.insert(vtx);

    return vertices;
}

void
getNodeCopies(Ptr<MeshSurface> surface, std::unordered_map<int, NodeCopies> & copies)
{
    auto & geom_surface = surface->geom_surface();

    std::vector<Ptr<MeshCurve>> all_curves;
    auto curves = surface->curves();
    all_curves.insert(all_curves.end(), curves.begin(), curves.end());
    auto emb_curves = surface->embedded_curves();
    all_curves.insert(all_curves.end(), emb_curves.begin(), emb_curves.end());

    std::set<Ptr<MeshCurve>> touched;

    // NOTE: this may not not be needed for meshing a single surface
    // if (edges.empty())
    //     edges.insert(edges.end(), gf->model()->firstEdge(), gf->model()->lastEdge());

    for (auto & crv : all_curves) {
        if (crv->is_mesh_degenerated())
            continue;

        auto & geom_curve = crv->geom_curve();
        std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan> e_vertices;
        for (auto & seg : crv->segments()) {
            auto v1 = seg.vertex(0);
            auto v2 = seg.vertex(1);
            e_vertices.insert(v1);
            e_vertices.insert(v2);
        }
        int direction = -1;
        if (geom_curve.is_seam(geom_surface)) {
            direction = 0;
            if (touched.find(crv) == touched.end())
                touched.insert(crv);
            else
                direction = 1;
        }
        // printf("model edge %lu %lu vertices\n", e->tag(), e_vertices.size());
        for (auto & v : e_vertices) {
            UVParam param;
            if (direction != -1) {
                auto u = reparam_mesh_vertex_on_edge(v, crv);
                param = reparam_on_surface(geom_surface, geom_curve, u);
            }
            else {
#if 0
                // TODO: if we ever have discrete surfaces
                // Hmm...
                if (!gf->haveParametrization() && gf->geomType() == GEntity::DiscreteSurface) {
                    param = SPoint2(v->x(), v->y());
                }
                else
                    reparamMeshVertexOnFace(v, gf, param);
#else
                param = reparam_mesh_vertex_on_surface(v, geom_surface);
#endif
            }
            auto it = copies.find(v->num());
            if (it == copies.end()) {
                NodeCopies c(v, param);
                copies.emplace(v->num(), c);
            }
            else {
                it->second.add_copy(param);
            }
        }
    }

    for (auto & v : surface->embedded_vertices()) {
        auto param = reparam_mesh_vertex_on_surface(v, geom_surface);
        NodeCopies c(v, param);
        copies.emplace(v->num(), c);
    }
}

int
delaunayEdgeCriterionPlaneIsotropic(PolyMesh::HalfEdge * he, void *)
{
    if (he->opposite == nullptr)
        return -1;
    auto * v0 = he->v;
    auto * v1 = he->next->v;
    auto * v2 = he->next->next->v;
    auto * v = he->opposite->next->next->v;

    // FIXME : should be oriented anyway !
    auto result = -incircle(v0->position, v1->position, v2->position, v->position);

    return (result > 0) ? 1 : 0;
}

PolyMesh
GFaceInitialMesh(Ptr<MeshSurface> surface,
                 bool recover /*, std::vector<double> * additional = nullptr*/)
{
    auto face_tag = surface->id();
    PolyMesh pm;

    std::unordered_map<int, NodeCopies> copies;
    getNodeCopies(surface, copies);

    BoundingBox3D bb;
    for (auto & [_, c] : copies) {
        for (std::size_t i = 0; i < c.n_copies; i++)
            bb += Point(c.uv[i].u, c.uv[i].v, 0.);
    }
    bb *= 1.1;
    pm.initialize_rectangle(bb.min().x, bb.max().x, bb.min().y, bb.max().y);
    PolyMesh::Face * f = pm.faces[0];
    for (auto & [vnum, cps] : copies) {
        for (std::size_t i = 0; i < cps.n_copies; i++) {
            auto uv = cps.uv[i];
            // find face in which lies x,y
            f = walk(f, uv);
            // split f and then swap edges to recover delaunayness
            pm.split_triangle(uv.u, uv.v, 0, f, delaunayEdgeCriterionPlaneIsotropic, nullptr);
            // remember node tags
            cps.id[i] = pm.vertices.size() - 1;
            pm.vertices[pm.vertices.size() - 1]->data = vnum;
        }
    }

    if (recover) {
        std::vector<Ptr<MeshCurve>> all_curves;
        auto curves = surface->curves();
        all_curves.insert(all_curves.end(), curves.begin(), curves.end());
        auto emb_curves = surface->embedded_curves();
        all_curves.insert(all_curves.end(), emb_curves.begin(), emb_curves.end());

        // NOTE: not sure if we really need this
        // if (edges.empty())
        //     edges.insert(edges.end(), surface->model()->firstEdge(),
        //     surface->model()->lastEdge());

        for (auto & crv : all_curves) {
            if (crv->is_mesh_degenerated())
                continue;

            for (auto & seg : crv->segments()) {
                auto c0 = copies.find(seg.vertex(0)->num());
                auto c1 = copies.find(seg.vertex(1)->num());
                if (c0 == copies.end() || c1 == copies.end())
                    Log::error("unable to find {} {} {} {}",
                               seg.vertex(0)->num(),
                               seg.vertex(1)->num(),
                               c0 == copies.end(),
                               c1 == copies.end());
                if (c0->second.n_copies > c1->second.n_copies) {
                    std::swap(c0, c1);
                }
                for (std::size_t j = 0; j < c0->second.n_copies; j++) {
                    PolyMesh::Vertex * v0 = pm.vertices[c0->second.id[j]];
                    PolyMesh::Vertex * v1 = pm.vertices[c1->second.closest(c0->second.uv[j])];
                    auto result = recover_edge(pm, v0, v1);
                    if (result.has_value()) {
                        auto he = pm.get_edge(v0, v1);
                        if (he) {
                            if (he->opposite)
                                he->opposite->data = crv->id();
                            he->data = crv->id();
                        }
                    }
                    else {
                        Log::warn("Impossible to recover edge {} {}",
                                  seg.vertex(0)->num(),
                                  seg.vertex(1)->num());
                    }
                }
            }
        }

        // color all PolyMesh::Faces
        // the first 4 vertices are "infinite vertices" --> color them with tag -2
        // meaning exterior
        auto * other_side = color(pm.vertices[0]->he, -2);
        // other_side is inthernal to the face --> color them with tag faceTag
        other_side = color(other_side, face_tag);
        // holes will be tagged -1

        // flip edges that have been scrambled
        int iter = 0;
        while (iter++ < 100) {
            int count = 0;
            for (auto & he : pm.hedges) {
                if (he->opposite && he->f->data == face_tag && he->opposite->f->data == face_tag) {
                    if (delaunayEdgeCriterionPlaneIsotropic(he, nullptr)) {
                        if (intersect(he->v,
                                      he->next->v,
                                      he->next->next->v,
                                      he->opposite->next->next->v)) {
                            pm.swap_edge(he);
                            count++;
                        }
                    }
                }
            }
            if (!count)
                break;
        }
    }

    // if (additional)
    //     addPoints(pm, *additional, bb);

    return pm;
}

void
build_bds_mesh(Ptr<MeshSurface> surface,
               BDS_Mesh & m,
               std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan> & all_vertices,
               std::map<Ptr<MeshVertexAbstract>, Ptr<BDS_Point>> & recoverMapInv)
{
    auto & geom_surface = surface->geom_surface();

    // std::vector<GEdge *> temp;
    // if (replacementEdges) {
    //     temp = gf->edges();
    //     gf->set(*replacementEdges);
    // }
    // recover and color so most of the code below can go away. Works also for
    // periodic faces
    auto pm = GFaceInitialMesh(surface, true);

    // if (replacementEdges) {
    //     gf->set(temp);
    // }

    std::map<int, Ptr<BDS_Point>> aaa;
    for (auto & vtx : all_vertices)
        aaa[vtx->num()] = recoverMapInv[vtx];

    for (int ip = 0; ip < 4; ip++) {
        auto * v = pm.vertices[ip];
        v->data = -ip - 1;
        auto pp = m.add_point(v->data, { v->position.x, v->position.y }, &geom_surface);
        auto g = m.add_geom(surface->id(), 2);
        pp->g_ = g;
        aaa[v->data] = pp;
    }

    for (size_t i = 0; i < pm.faces.size(); i++) {
        auto * he = pm.faces[i]->he;
        auto a = he->v->data;
        auto b = he->next->v->data;
        auto c = he->next->next->v->data;
        auto p1 = aaa[a];
        auto p2 = aaa[b];
        auto p3 = aaa[c];
        if (p1 && p2 && p3)
            m.add_triangle(p1->id(), p2->id(), p3->id());
    }
}

struct SwapQuad {
    std::array<int, 4> v;

    bool
    operator<(const SwapQuad & o) const
    {
        if (this->v[0] < o.v[0])
            return true;
        if (this->v[0] > o.v[0])
            return false;
        if (this->v[1] < o.v[1])
            return true;
        if (this->v[1] > o.v[1])
            return false;
        if (this->v[2] < o.v[2])
            return true;
        if (this->v[2] > o.v[2])
            return false;
        if (this->v[3] < o.v[3])
            return true;
        return false;
    }

    SwapQuad(Ptr<MeshVertexAbstract> v1,
             Ptr<MeshVertexAbstract> v2,
             Ptr<MeshVertexAbstract> v3,
             Ptr<MeshVertexAbstract> v4)
    {
        this->v = { v1->num(), v2->num(), v3->num(), v4->num() };
        std::sort(v.begin(), v.end());
    }

    SwapQuad(int v1, int v2, int v3, int v4)
    {
        this->v = { v1, v2, v3, v4 };
        std::sort(v.begin(), v.end());
    }
};

bool
recoverEdge(BDS_Mesh & m,
            Ptr<MeshSurface> surface,
            Ptr<MeshCurve> edge,
            std::map<Ptr<MeshVertexAbstract>, Ptr<BDS_Point>> & recoverMapInv,
            std::set<EdgeToRecover> * e2r,
            std::set<EdgeToRecover> * notRecovered,
            int pass)
{
    Optional<BDS_GeomEntity> g;
    if (pass == 2)
        g = m.add_geom(edge->id(), 1);

    auto & geom_curve = edge->geom_curve();
    bool fatally_failed = false;

    for (auto & seg : edge->segments()) {
        auto vstart = seg.vertex(0);
        auto vend = seg.vertex(1);
        auto itpstart = recoverMapInv.find(vstart);
        auto itpend = recoverMapInv.find(vend);
        if (itpstart != recoverMapInv.end() && itpend != recoverMapInv.end()) {
            auto pstart = itpstart->second;
            auto pend = itpend->second;
            if (pass == 1)
                e2r->insert(EdgeToRecover(pstart->id(), pend->id(), &geom_curve));
            else {
                auto e =
                    m.recover_edge(pstart->id(), pend->id(), fatally_failed, e2r, notRecovered);
                if (e.has_value())
                    e.value()->g_ = g;
                else {
                    if (fatally_failed) {
                        Log::error("Unable to recover the edge on curve {} (on surface {})",
                                   edge->id(),
                                   surface->id());
                    }
                    return !fatally_failed;
                }
            }
        }
    }

    auto bnd_vtxs = edge->bounding_vertices();
    if (pass == 2 && not bnd_vtxs[0].is_null()) {
        auto vstart = bnd_vtxs[0];
        auto vend = bnd_vtxs[1];
        auto itpstart = recoverMapInv.find(vstart);
        auto itpend = recoverMapInv.find(vend);
        if (itpstart != recoverMapInv.end() && itpend != recoverMapInv.end()) {
            auto pstart = itpstart->second;
            auto pend = itpend->second;
            if (!pstart->g_) {
                auto g0 = m.add_geom(pstart->id(), 0);
                pstart->g_ = g0;
            }
            if (!pend->g_) {
                auto g0 = m.add_geom(pend->id(), 0);
                pend->g_ = g0;
            }
        }
    }

    return true;
}

bool
edgeSwapTestDelaunayAniso(BDS_Edge * e, Ptr<MeshSurface> surface, std::set<SwapQuad> & configs)
{
    if (!e->p1_->config_modified() && !e->p2_->config_modified())
        return false;

    if (e->num_faces() != 2)
        return false;

    auto op = e->opposite_of();

    SwapQuad sq(e->p1_->id(), e->p2_->id(), op[0]->id(), op[1]->id());
    if (configs.find(sq) != configs.end())
        return false;
    configs.insert(sq);

    auto edge_center = 0.5 * (e->p1_->uv() + e->p2_->uv());

    auto p1 = e->p1_->uv();
    auto p2 = e->p2_->uv();
    auto p3 = op[0]->uv();
    auto p4 = op[1]->uv();
    auto metric = Metric::build(surface->geom_surface(), edge_center);
    return in_circum_circle_aniso(p1, p2, p3, p4, metric);
}

int
delaunayize_bds(Ptr<MeshSurface> surface, BDS_Mesh & mesh)
{
    int nb_swap = 0;
    std::set<SwapQuad> configs;
    while (1) {
        std::size_t nsw = 0;
        for (auto & edge : mesh.edges()) {
            if (edge->active()) {
                if (edgeSwapTestDelaunayAniso(edge.get(), surface, configs)) {
                    if (mesh.swap_edge(edge.get(), BDS_SwapEdgeTestQuality(false))) {
                        ++nsw;
                    }
                }
            }
        }
        nb_swap += nsw;
        if (!nsw)
            return nb_swap;
    }
}

// TODO: rename this
void
BDS2GMSH(BDS_Mesh & m,
         Ptr<MeshSurface> surface,
         std::map<Ptr<BDS_Point>, Ptr<MeshVertexAbstract>, PointLessThan> & recoverMap)
{
    auto geom_surface = surface->geom_surface();
    for (auto & tri : m.triangles()) {
        if (tri->active()) {
            auto n = tri->get_nodes().value();

            Ptr<MeshVertexAbstract> v[3] = { nullptr, nullptr, nullptr };
            for (int i = 0; i < 3; i++) {
                if (n[i] == nullptr)
                    continue;
                if (recoverMap.find(n[i]) == recoverMap.end()) {
                    auto sv = Ptr<MeshSurfaceVertex>::alloc(geom_surface, n[i]->uv());
                    surface->add_vertex(sv);
                    v[i] = sv;
                    recoverMap[n[i]] = v[i];
                }
                else
                    v[i] = recoverMap[n[i]];
            }
            // when a singular point is present, degenerated triangles may be
            // created, for example on a sphere that contains one pole
            if (v[0] != v[1] && v[0] != v[2] && v[1] != v[2])
                surface->add_triangle({ v[0], v[1], v[2] });
        }
    }
}

int
insertVertexB(std::list<edgeXface> & shell,
              std::list<Triangle *> & cavity,
              bool force,
              Ptr<MeshVertexAbstract> v,
              std::set<Triangle *, CompareTrianglePtr> & all_tris,
              std::set<Triangle *, CompareTrianglePtr> * active_tris,
              BidimMeshData & data,
              Triangle ** oneNewTriangle,
              bool verifyStarShapeness = true)
{
    if (cavity.size() == 1)
        return -1;

    if (shell.size() != cavity.size() + 2)
        return -2;

    double EPS = verifyStarShapeness ? 1.e-12 : 1.e12;

    // check that volume is conserved
    double newVolume = 0.0;
    double newMinQuality = 2.0;

    double oldVolume =
        std::accumulate(begin(cavity),
                        end(cavity),
                        0.0,
                        [&](double volume, Triangle * const triangle) {
                            return volume + std::abs(getSurfUV(triangle->tri(), data));
                        });

    auto ** newTris = new Triangle *[shell.size()];

    std::vector<Triangle *> new_cavity;

    int k = 0;

    bool onePointIsTooClose = false;

    for (auto it = shell.begin(); it != shell.end(); ++it) {
        Ptr<MeshVertexAbstract> v0, v1;
        if (it->ori > 0) {
            v0 = it->vertex(0);
            v1 = it->vertex(1);
        }
        else {
            v0 = it->vertex(1);
            v1 = it->vertex(0);
        }
        auto t = MeshElement::Tri3({ v0, v1, v });
        auto index0 = data.index(t.vertex(0));
        auto index1 = data.index(t.vertex(1));
        auto index2 = data.index(t.vertex(2));
        constexpr double ONE_THIRD = 1. / 3.;
        double lc =
            ONE_THIRD * (data.v_sizes[index0] + data.v_sizes[index1] + data.v_sizes[index2]);

        auto circ_radius = circum_radius_euclidian(t, lc);
        auto * t4 = new Triangle(t, circ_radius);

        if (oneNewTriangle) {
            force = true;
            *oneNewTriangle = t4;
        }

        double d1 = utils::distance(v0->point(), v->point());
        double d2 = utils::distance(v1->point(), v->point());
        double d3 = utils::distance(v0->point(), v1->point());
        double d4 = MAX_LC;
        // avoid angles that are too obtuse
        double cosv = ((d1 * d1 + d2 * d2 - d3 * d3) / (2. * d1 * d2));

        if (v0->geom_shape().dim() != 2 && v1->geom_shape().dim() != 2) {
            auto v0v1 = v1->point() - v0->point();
            auto v0v = v->point() - v0->point();
            auto pv = cross_product(v0v1, v0v);
            d4 = pv.magnitude() / d3;
        }

        if ((d1 < lc * .5 || d2 < lc * .5 || d4 < lc * .4 || cosv < -.9999) && !force) {
            onePointIsTooClose = true;
        }

        newTris[k++] = t4;
        // all new triangles are pushed front in order to be able to destroy them if
        // the cavity is not star shaped around the new vertex.
        new_cavity.push_back(t4);

        auto * other_side = it->t1->neighbor(it->i1);
        if (other_side)
            new_cavity.push_back(other_side);

        auto ss = std::abs(getSurfUV(t4->tri(), data));
        if (ss < 1.e-25)
            ss = MAX_LC;

        newVolume += ss;

        auto tri_gamma =
            Tri3::gamma(t.vertex(0)->point(), t.vertex(1)->point(), t.vertex(1)->point());
        newMinQuality = std::min(newMinQuality, tri_gamma);
    }

    std::vector<edgeXface> conn;

    // for adding a point we require that the area remains the same after addition
    // of the point, and that the point is not too close to an edge
    if (std::abs(oldVolume - newVolume) < EPS * oldVolume && !onePointIsTooClose) {
        connectTris(new_cavity.begin(), new_cavity.end(), conn);
        // 30 % of the time is spent here!
        all_tris.insert(newTris, newTris + shell.size());
        if (active_tris) {
            for (auto i = new_cavity.begin(); i != new_cavity.end(); ++i) {
                int active_edge;
                if (isActive(*i, LIMIT, active_edge) && (*i)->radius() > LIMIT) {
                    if ((*active_tris).find(*i) == (*active_tris).end())
                        (*active_tris).insert(*i);
                }
            }
        }
        delete[] newTris;
        return 1;
    }
    else {
        // the cavity is NOT star shaped
        std::for_each(begin(cavity), end(cavity), [](Triangle * triangle) {
            triangle->set_deleted(false);
        });
        for (std::size_t i = 0; i < shell.size(); i++) {
            delete newTris[i];
        }
        delete[] newTris;

        if (std::abs(oldVolume - newVolume) > EPS * oldVolume)
            return -3;
        if (onePointIsTooClose)
            return -4;
        return -5;
    }
}

#if 0

bool
invMapXY(const MeshElement & t, Ptr<MeshVertexAbstract> v)
{
    auto v0 = t.vertex(0);
    auto v1 = t.vertex(1);
    auto v2 = t.vertex(2);
    auto pt0 = v0->point();
    auto pt1 = v1->point();
    auto pt2 = v2->point();

    std::array<std::array<double, 2>, 2> mat;
    std::array<double, 2> b;
    mat[0][0] = pt1.x - pt0.x;
    mat[0][1] = pt2.x - pt0.x;
    mat[1][0] = pt1.y - pt0.y;
    mat[1][1] = pt2.y - pt0.y;

    auto pt = v->point();
    b[0] = pt.x - pt0.x;
    b[1] = pt.y - pt0.y;
    auto uv = sys2x2(mat, b).value();

    double tol = 1.e-6;
    if (uv[0] >= -tol && uv[1] >= -tol && uv[0] <= 1. + tol && uv[1] <= 1. + tol &&
        1. - uv[0] - uv[1] > -tol) {
        return true;
    }
    return false;
}

Triangle *
search4Triangle(Triangle * t, Ptr<MeshVertexAbstract> v, int maxx, int & ITER)
{
    bool inside = invMapXY(t->tri(), v);
    UVParam q1(v->point().x, v->point().y);
    if (inside)
        return t;
    while (1) {
        auto bctr = barycenter(t->tri());
        UVParam q2(bctr.x, bctr.y);
        int i;
        for (i = 0; i < 3; i++) {
            int i1 = i == 0 ? 2 : i - 1;
            int i2 = i;
            auto v1 = t->tri().vertex(i1);
            auto v2 = t->tri().vertex(i2);
            UVParam p1(v1->point().x, v1->point().y);
            UVParam p2(v2->point().x, v2->point().y);
            if (intersection_segments_2(p1, p2, q1, q2))
                break;
        }
        if (i >= 3)
            break;
        t = t->neighbor(i);
        if (!t)
            break;
        bool inside = invMapXY(t->tri(), v);
        if (inside)
            return t;
        if (ITER++ > .5 * maxx)
            break;
    }
    return nullptr;
}

#endif

Optional<Triangle *>
search4Triangle(Triangle * t,
                UVParam pt,
                BidimMeshData & data,
                std::set<Triangle *, CompareTrianglePtr> & all_tris,
                bool force = false)
{
    auto [uv, inside] = invMapUV(t->tri(), pt, data, 1.e-8);
    if (inside)
        return t;

    UVParam q1 = pt;
    std::size_t ITER = 0;
    while (1) {
        auto index0 = data.index(t->tri().vertex(0));
        auto index1 = data.index(t->tri().vertex(1));
        auto index2 = data.index(t->tri().vertex(2));
        UVParam q2 = 1. / 3. * (data.uv[index0] + data.uv[index1] + data.uv[index2]);
        int i;
        for (i = 0; i < 3; i++) {
            auto i1 = data.index(t->tri().vertex(i == 0 ? 2 : i - 1));
            auto i2 = data.index(t->tri().vertex(i));
            UVParam p1 = data.uv[i1];
            UVParam p2 = data.uv[i2];
            if (intersection_segments_2(p1, p2, q1, q2))
                break;
        }
        if (i >= 3)
            throw Exception("impossible");

        t = t->neighbor(i);
        if (!t)
            break;

        auto [uv, inside] = invMapUV(t->tri(), pt, data, 1.e-8);
        if (inside)
            return t;

        if (ITER++ > all_tris.size())
            break;
    }

    if (!force)
        return std::nullopt; // FIXME: removing this leads to horrible performance

    for (auto & tri : all_tris) {
        if (not tri->is_deleted()) {
            auto [uv, inside] = invMapUV(tri->tri(), pt, data, 1.e-8);
            if (inside)
                return tri;
        }
    }
    return std::nullopt;
}

bool
insertAPoint(Ptr<MeshSurface> surface,
             std::set<Triangle *, CompareTrianglePtr>::iterator it,
             UVParam center,
             Metric metric,
             BidimMeshData & data,
             std::set<Triangle *, CompareTrianglePtr> & all_tris,
             std::set<Triangle *, CompareTrianglePtr> * active_tris = nullptr,
             Triangle * worst = nullptr,
             Triangle ** one_new_triangle = nullptr,
             bool test_star_shapeness = false)
{
    if (worst) {
        it = all_tris.find(worst);
        if (worst != *it) {
            Log::error("Could not insert point");
            return false;
        }
    }
    else
        worst = *it;

    Optional<Triangle *> ptin;
    std::list<edgeXface> shell;
    std::list<Triangle *> cavity;

    // if the point is able to break the bad triangle "worst"
    if (inCircumCircleAniso(worst->tri(), center, metric, data)) {
        recurFindCavityAniso(surface, shell, cavity, metric, center, worst, data);
        for (auto & t : cavity) {
            auto [uv, inside] = invMapUV(t->tri(), center, data, 1.e-8);
            if (inside) {
                ptin = t;
                break;
            }
        }
    }
    else {
        ptin = search4Triangle(worst, center, data, all_tris, one_new_triangle ? true : false);
        if (ptin.has_value()) {
            recurFindCavityAniso(surface, shell, cavity, metric, center, ptin.value(), data);
        }
    }

    if (ptin.has_value()) {
        // we use here local coordinates as real coordinates x,y and z will be computed hereafter
        auto & geom_surface = surface->geom_surface();
        auto v = Ptr<MeshSurfaceVertex>::alloc(geom_surface, center);
        auto lc = surface->mesh_size_at_param(center);
        data.add_vertex(v, center, lc);

        int result = insertVertexB(shell,
                                   cavity,
                                   false,
                                   v,
                                   all_tris,
                                   active_tris,
                                   data,
                                   one_new_triangle,
                                   test_star_shapeness);

        if (result != 1) {
            if (result == -1)
                Log::debug("Point {} cannot be inserted because cavity if of size 1", center);
            if (result == -2)
                Log::debug("Point {} cannot be inserted because euler formula is not fulfilled",
                           center);
            if (result == -3)
                Log::debug("Point {} cannot be inserted because cavity is not star shaped", center);
            if (result == -4)
                Log::debug("Point {} cannot be inserted because it is too close to another point",
                           center);
            if (result == -5)
                Log::debug(
                    "Point {} cannot be inserted because it is out of the parametric domain)",
                    center);

            all_tris.erase(it);
            worst->force_radius(-1);
            all_tris.insert(worst);
            for (auto & tri : cavity)
                tri->set_deleted(false);
            return false;
        }
        else {
            surface->add_vertex(v);
            return true;
        }
    }
    else {
        for (auto & tri : cavity)
            tri->set_deleted(false);
        all_tris.erase(it);
        worst->force_radius(0);
        all_tris.insert(worst);
        return false;
    }
}

void
reverse_triangle(MeshElement & tri)
{
    assert(tri.type() == ElementType::TRI3);
    tri.swap_vertices(1, 2);
}

void
computeEquivalences(Ptr<MeshSurface> surface, BidimMeshData & data)
{
    if (data.equivalence == nullptr)
        return;

    std::vector<MeshElement> new_tris;
    for (auto & tri : surface->triangles()) {
        std::array<Ptr<MeshVertexAbstract>, 3> v;
        for (int j = 0; j < Tri3::N_VERTICES; j++) {
            v[j] = tri.vertex(j);
            auto it = data.equivalence->find(v[j]);
            if (it != data.equivalence->end()) {
                v[j] = it->second;
            }
        }
        if (v[0] != v[1] && v[0] != v[2] && v[2] != v[1])
            new_tris.push_back(MeshElement::Tri3({ v[0], v[1], v[2] }));
    }
    surface->set_triangles(new_tris);
}

struct EquivalentTriangle {
private:
    MeshElement _t;
    std::array<Ptr<MeshVertexAbstract>, 3> _v;

public:
    EquivalentTriangle(const MeshElement & t,
                       std::map<Ptr<MeshVertexAbstract>, Ptr<MeshVertexAbstract>> * equivalence) :
        _t(t)
    {
        for (int i = 0; i < Tri3::N_VERTICES; i++) {
            auto v = t.vertex(i);
            auto it = equivalence->find(v);
            if (it == equivalence->end())
                this->_v[i] = v;
            else
                this->_v[i] = it->second;
        }
        std::sort(this->_v.begin(), this->_v.end());
    }

    bool
    operator<(const EquivalentTriangle & other) const
    {
        for (int i = 0; i < Tri3::N_VERTICES; i++) {
            if (other._v[i].get() > this->_v[i].get())
                return true;
            if (other._v[i].get() < this->_v[i].get())
                return false;
        }
        return false;
    }

    const MeshElement &
    elem() const
    {
        return this->_t;
    }
};

bool
computeEquivalentTriangles(Ptr<MeshSurface> surface,
                           std::map<Ptr<MeshVertexAbstract>, Ptr<MeshVertexAbstract>> * equivalence)
{
    if (equivalence == nullptr)
        return false;

    std::vector<MeshElement> WTF;
    std::set<EquivalentTriangle> eqTs;
    for (auto & tri : surface->triangles()) {
        EquivalentTriangle et(tri, equivalence);
        auto iteq = eqTs.find(et);
        if (iteq == eqTs.end())
            eqTs.insert(et);
        else {
            WTF.push_back(iteq->elem());
            WTF.push_back(tri);
        }
    }

    if (not WTF.empty()) {
        Log::info("{} triangles are equivalent", WTF.size());
        return true;
    }
    else
        return false;
}

void
splitEquivalentTriangles(Ptr<MeshSurface> surface, BidimMeshData & data)
{
    computeEquivalentTriangles(surface, data.equivalence);
}

/// Compute normal of a triangle
///
/// @param t Triangle
/// @param data BidimMeshData
Vector
compute_normal(const MeshElement & t, const BidimMeshData & data)
{
    auto v0 = t.vertex(0);
    auto v1 = t.vertex(1);
    auto v2 = t.vertex(2);

    auto index0 = data.index(v0);
    auto index1 = data.index(v1);
    auto index2 = data.index(v2);
    return normal(data.uv[index0], data.uv[index1], data.uv[index2]);
}

void
transferDataStructure(Ptr<MeshSurface> surface,
                      std::set<Triangle *, CompareTrianglePtr> & all_tris,
                      BidimMeshData & data)
{
    while (not all_tris.empty()) {
        auto worst = *all_tris.begin();
        if (not worst->is_deleted())
            surface->add_element(worst->tri());
        delete worst;
        all_tris.erase(all_tris.begin());
    }

    // make sure all the triangles are oriented in the same way in
    // parameter space (it would be nicer to change the actual algorithm
    // to ensure that we create correctly-oriented triangles in the
    // first place)

    if (surface->triangles().size() > 1) {
        auto & t1 = surface->triangles()[0];
        auto n1 = compute_normal(t1, data);

        for (std::size_t j = 1; j < surface->triangles().size(); j++) {
            auto & tj = surface->triangles()[j];
            auto nj = compute_normal(tj, data);

            // orient the bignou
            if (dot_product(n1, nj) < 0.0)
                reverse_triangle(tj);
        }
    }
    splitEquivalentTriangles(surface, data);
    computeEquivalences(surface, data);
}

void
delete_unused_vertices(Ptr<MeshSurface> /*surface*/)
{
#if 0
    std::set<Ptr<MeshVertexAbstract>, MeshVertexPtrLessThan> allverts;
    for(std::size_t i = 0; i < surface->triangles().size(); i++) {
      for(int j = 0; j < 3; j++) {
        if(gf->triangles[i]->getVertex(j)->onWhat() == gf)
          allverts.insert(gf->triangles[i]->getVertex(j));
      }
    }
    for(std::size_t i = 0; i < gf->quadrangles.size(); i++) {
      for(int j = 0; j < 4; j++) {
        if(gf->quadrangles[i]->getVertex(j)->onWhat() == gf)
          allverts.insert(gf->quadrangles[i]->getVertex(j));
      }
    }
    for(std::size_t i = 0; i < gf->mesh_vertices.size(); i++) {
      if(allverts.find(gf->mesh_vertices[i]) == allverts.end())
        delete gf->mesh_vertices[i];
    }
    gf->mesh_vertices.clear();
    gf->mesh_vertices.insert(gf->mesh_vertices.end(), allverts.begin(),
                             allverts.end());
#endif
}

// ---

double
lengthMetric(UVParam p, UVParam q, Metric metric)
{
    return std::sqrt((p.u - q.u) * metric[0] * (p.u - q.u) +
                     2 * (p.u - q.u) * metric[1] * (p.v - q.v) +
                     (p.v - q.v) * metric[2] * (p.v - q.v));
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

std::tuple<double, UVParam, Metric>
optimalPointFrontal(Ptr<MeshSurface> surface,
                    Triangle * worst,
                    int active_edge,
                    BidimMeshData & data)
{
    auto & base = worst->tri();
    auto center = circ_uv(base, data);
    int index0 = data.index(base.vertex(0));
    int index1 = data.index(base.vertex(1));
    int index2 = data.index(base.vertex(2));
    UVParam pa = 1. / 3. * (data.uv[index0] + data.uv[index1] + data.uv[index2]);
    auto metric = Metric::build(surface->geom_surface(), pa);
    double r2;
    std::tie(center, r2) = circum_center_metric(worst->tri(), metric, data);
    // compute the middle point of the edge
    int ip1 = active_edge - 1 < 0 ? 2 : active_edge - 1;
    int ip2 = active_edge;

    index0 = data.index(base.vertex(ip1));
    index1 = data.index(base.vertex(ip2));
    UVParam midpoint = 0.5 * (data.uv[index0] + data.uv[index1]);

    // now we have the edge center and the center of the circumcircle, we try to
    // find a point that would produce a perfect triangle while connecting the 2
    // points of the active edge
    UVParam dir = center - midpoint;
    double norm = std::sqrt(dir.u * dir.u + dir.v * dir.v);
    dir.u /= norm;
    dir.v /= norm;
    const double RATIO = std::sqrt(dir.u * dir.u * metric[0] + 2 * dir.v * dir.u * metric[1] +
                                   dir.v * dir.v * metric[2]);

    const double rhoM1 = 0.5 * (data.v_sizes[index0] + data.v_sizes[index1]); // * RATIO;
    // const double rhoM2 = 0.5 * (data.vSizesBGM[index0] + data.vSizesBGM[index1]); // * RATIO;
    // const double rhoM = Extend1dMeshIn2dSurfaces(gf) ? std::min(rhoM1, rhoM2) : rhoM2;
    // const double rhoM_hat = rhoM;
    const double rhoM_hat = rhoM1;

    const double q = lengthMetric(center, midpoint, metric);
    const double d = rhoM_hat * std::sqrt(3.0) * 0.5;

    // d is corrected in a way that the mesh size is computed at point newPoint

    const double L = std::min(d, q);

    auto newPoint = midpoint + L / RATIO * dir;
    // newPoint[0] = midpoint[0] + L * dir[0] / RATIO;
    // newPoint[1] = midpoint[1] + L * dir[1] / RATIO;

    return { L, newPoint, metric };
}

std::tuple<bool, int>
pointInsideParametricDomain(std::vector<UVParam> & bnd, UVParam p, UVParam out)
{
    int count = 0;
    for (size_t i = 0; i < bnd.size(); i += 2) {
        UVParam p1 = bnd[i];
        UVParam p2 = bnd[i + 1];
        double a = orient2d(p1, p2, p);
        double b = orient2d(p1, p2, out);
        if (a * b < 0) {
            a = orient2d(p, out, p1);
            b = orient2d(p, out, p2);
            if (a * b < 0)
                count++;
        }
    }
    if (count % 2 == 0)
        return { false, count };
    return { true, count };
}

class surfaceFunctor {
public:
    virtual ~surfaceFunctor() {}
    virtual Point operator()(double u, double v) const = 0;
};

class curveFunctor {
public:
    virtual ~curveFunctor() {}
    virtual Point operator()(double t) const = 0;
};

class curveFunctorCircle : public curveFunctor {
    Vector n1_, n2_;
    Point middle_;
    double d_;

public:
    curveFunctorCircle(Vector n1, Vector n2, Point middle, double d) :
        n1_(n1),
        n2_(n2),
        middle_(middle),
        d_(d)
    {
    }

    Point
    operator()(double t) const
    {
        auto dir = this->d_ * (this->n1_ * std::cos(t) + this->n2_ * std::sin(t));
        return this->middle_ + dir;
    }
};

class surfaceFunctorGFace : public surfaceFunctor {
    const GeomSurface & gf_;

public:
    surfaceFunctorGFace(const GeomSurface & gf) : gf_(gf) {}

    virtual Point
    operator()(double u, double v) const
    {
        return this->gf_.point({ u, v });
    }
};

bool
newton_fd(bool (*func)(const Eigen::VectorXd &, Eigen::VectorXd &, void *),
          Eigen::VectorXd & x,
          void * data,
          double relax = 1.,
          double tolx = 1e-6)
{
    const int MAXIT = 100;
    const double EPS = 1.e-4;
    const int N = x.size();

    Eigen::MatrixXd J(N, N);
    Eigen::VectorXd f(N), feps(N), dx(N);

    for (int iter = 0; iter < MAXIT; iter++) {
        if (x.norm() > 1.e6)
            return false;
        if (!func(x, f, data)) {
            return false;
        }

        bool isZero = false;
        for (int k = 0; k < N; k++) {
            if (f(k) == 0.)
                isZero = true;
            else
                isZero = false;
            if (isZero == false)
                break;
        }
        if (isZero)
            break;

        for (int j = 0; j < N; j++) {
            double h = EPS * fabs(x(j));
            if (h == 0.)
                h = EPS;
            x(j) += h;
            if (!func(x, feps, data)) {
                return false;
            }
            for (int i = 0; i < N; i++) {
                J(i, j) = (feps(i) - f(i)) / h;
            }
            x(j) -= h;
        }

        if (N == 1)
            dx(0) = f(0) / J(0, 0);
        else {
            Eigen::FullPivLU<Eigen::MatrixXd> lu_full(J);
            dx = lu_full.solve(f);
        }

        for (int i = 0; i < N; i++)
            x(i) -= relax * dx(i);

        if (dx.norm() < tolx) {
            return true;
        }
    }
    return false;
}

bool _kaboom(const Eigen::VectorXd & uvt, Eigen::VectorXd & res, void * _data);

struct intersectCurveSurfaceData {
    const curveFunctor & c;
    const surfaceFunctor & s;
    const double epsilon;
    intersectCurveSurfaceData(const curveFunctor & _c,
                              const surfaceFunctor & _s,
                              const double & eps) :
        c(_c),
        s(_s),
        epsilon(eps)
    {
    }

    Optional<Point>
    apply(Point newPoint)
    {
        try {
            Eigen::VectorXd uvt(3);
            uvt[0] = newPoint.x;
            uvt[1] = newPoint.y;
            uvt[2] = newPoint.z;
            Eigen::VectorXd res(3);
            _kaboom(uvt, res, this);
            // printf("start with %12.5E\n",res.norm());
            if (res.norm() < epsilon)
                return newPoint;

            if (newton_fd(_kaboom, uvt, this)) {
                // printf("--- CONVERGED -----------\n");
                // printf("newton done\n");
                newPoint.x = uvt(0);
                newPoint.y = uvt(1);
                newPoint.z = uvt(2);
                return newPoint;
            }
        }
        catch (...) {
            // printf("intersect curve surface failed !\n");
        }
        // printf("newton failed\n");
        return std::nullopt;
    }
};

bool
_kaboom(const Eigen::VectorXd & uvt, Eigen::VectorXd & res, void * _data)
{
    intersectCurveSurfaceData * data = (intersectCurveSurfaceData *) _data;
    auto s = data->s(uvt(0), uvt(1));
    auto c = data->c(uvt(2));
    res(0) = s.x - c.x;
    res(1) = s.y - c.y;
    res(2) = s.z - c.z;
    return true;
}

Optional<Point>
intersectCurveSurface(curveFunctor & c, surfaceFunctor & s, Point uvt, double epsilon)
{
    intersectCurveSurfaceData data(c, s, epsilon);
    return data.apply(uvt);
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

std::tuple<bool, UVParam, Metric>
optimalPointFrontalB(Ptr<MeshSurface> surface,
                     Triangle * worst,
                     int active_edge,
                     BidimMeshData & data)
{
    // as a starting point, let us use the "fast algo"
    auto [d, newPoint, metric] = optimalPointFrontal(surface, worst, active_edge, data);
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
        return { true, newPoint, metric };
    auto n2 = cross_product(n1, v1v2);
    n1.normalize();
    n2.normalize();
    // we look for a point that is
    // P = d * (n1 std::cos(t) + n2 std::sin(t)) that is on the surface
    // so we have to find t, starting with t = 0
    //  return true;

    Point uvt(newPoint.u, newPoint.v, 0.0);
    curveFunctorCircle cc(n2, n1, middle, d);
    surfaceFunctorGFace ss(surface->geom_surface());

    auto ppp = intersectCurveSurface(cc, ss, uvt, d * 1.e-8);
    if (ppp.has_value()) {
        auto pt = ppp.value();
        return { true, { pt.x, pt.y }, metric };
    }

    return { true, newPoint, metric };
}

} // namespace

void
bowyer_watson(Ptr<MeshSurface> surface, int MAXPNT)
{
    std::set<Triangle *, CompareTrianglePtr> all_tris;
    BidimMeshData data;
    if (!buildMeshGenerationDataStructures(surface, all_tris, data)) {
        Log::error("Invalid meshing data structure");
        return;
    }

    if (all_tris.empty()) {
        Log::error("No triangles in initial mesh");
        return;
    }

    int iter = 0;
    while (true) {
        auto * worst = *all_tris.begin();
        if (worst->is_deleted()) {
            delete worst;
            all_tris.erase(all_tris.begin());
        }
        else {
            if (iter++ % 5000 == 0) {
                Log::debug("{} points created -- worst tri radius is {}",
                           data.v_sizes.size(),
                           worst->radius());
            }
            // VERIFY STOP !!!
            if (worst->radius() < 0.5 * std::sqrt(2.0) || (int) data.v_sizes.size() > MAXPNT) {
                break;
            }

            const auto & base = worst->tri();
            int index0 = data.index(base.vertex(0));
            int index1 = data.index(base.vertex(1));
            int index2 = data.index(base.vertex(2));
            auto pa = 1. / 3. * (data.uv[index0] + data.uv[index1] + data.uv[index2]);

            auto metric = Metric::build(surface->geom_surface(), pa);
            auto [ctr2, r2] = circum_center_metric(worst->tri(), metric, data);
            insertAPoint(surface, all_tris.begin(), ctr2, metric, data, all_tris);
        }
    }

    transferDataStructure(surface, all_tris, data);
}

void
bowyer_watson_frontal(Ptr<MeshSurface> surface,
                      std::map<Ptr<MeshVertexAbstract>, Ptr<MeshVertexAbstract>> * equivalence,
                      std::map<Ptr<MeshVertexAbstract>, UVParam> * parametricCoordinates,
                      std::vector<UVParam> * true_boundary)
{
    std::set<Triangle *, CompareTrianglePtr> AllTris;
    std::set<Triangle *, CompareTrianglePtr> ActiveTris;
    BidimMeshData DATA(equivalence, parametricCoordinates);
    bool testStarShapeness = true;

    // std::set<GEntity *> degenerated;
    // getDegeneratedVertices(surface, degenerated);

    if (!buildMeshGenerationDataStructures(surface, AllTris, DATA)) {
        Log::error("Invalid meshing data structure");
        return;
    }

    int ITER = 0, active_edge;
    // compute active triangle
    auto it = AllTris.begin();
    for (; it != AllTris.end(); ++it) {
        if (isActive(*it, LIMIT, active_edge))
            ActiveTris.insert(*it);
        else if ((*it)->radius() < LIMIT)
            break;
    }

    auto & geom_surface = surface->geom_surface();
    auto [ru_lo, ru_hi] = geom_surface.param_range(0);
    auto [rv_lo, rv_hi] = geom_surface.param_range(1);
    UVParam FAR(2 * ru_hi, 2 * rv_hi);

    // insert points
    while (true) {
        if (!ActiveTris.size())
            break;
        auto * worst = (*ActiveTris.begin());
        ActiveTris.erase(ActiveTris.begin());

        if (!worst->is_deleted() && isActive(worst, LIMIT, active_edge) &&
            worst->radius() > LIMIT) {
            if (ITER++ % 5000 == 0)
                Log::debug("{} points created -- Worst tri radius is {}",
                           surface->surface_vertices().size(),
                           worst->radius());
            // UVParam newPoint;
            // Metric metric;
            auto [success, newPoint, metric] =
                optimalPointFrontalB(surface, worst, active_edge, DATA);
            if (success) {
                if (true_boundary == nullptr) {
                    insertAPoint(surface,
                                 AllTris.end(),
                                 newPoint,
                                 metric,
                                 DATA,
                                 AllTris,
                                 &ActiveTris,
                                 worst,
                                 nullptr,
                                 testStarShapeness);
                }
                else {
                    UVParam NP = newPoint;
                    auto [inside, nnnn] = pointInsideParametricDomain(*true_boundary, NP, FAR);
                    if (inside)
                        insertAPoint(surface,
                                     AllTris.end(),
                                     newPoint,
                                     metric,
                                     DATA,
                                     AllTris,
                                     &ActiveTris,
                                     worst,
                                     nullptr,
                                     testStarShapeness);
                }
            }
        }
    }

    transferDataStructure(surface, AllTris, DATA);
}

SchemeDelaunay::SchemeDelaunay(const std::string & name) : Scheme(name) {}

bool
SchemeDelaunay::mesh_generation(Ptr<MeshSurface> surface,
                                Span<Ptr<MeshCurve>> curves,
                                bool only_initial_mesh)
{
    ///
    auto [all_vertices, boundary] = build_vertices(surface, curves);
    if (not boundary.empty()) {
        Log::error("The 1D mesh seems not to be forming a closed loop ({} boundary "
                   "nodes are considered once)",
                   boundary.size());
        return false;
    }

    auto emb_edge_vtxs = build_embedded_vertices(surface);
    all_vertices.insert(emb_edge_vtxs.begin(), emb_edge_vtxs.end());

    if (all_vertices.size() < 3) {
        Log::warn("Mesh generation of surface {} skipped: only {} nodes on the boundary",
                  surface->id(),
                  all_vertices.size());
        return true;
    }
    else if (all_vertices.size() == 3) {
        std::array<Ptr<MeshVertexAbstract>, 3> tri;
        std::copy(all_vertices.begin(), all_vertices.end(), tri.begin());
        surface->add_triangle(tri);
        return true;
    }

    ///

    const BDS_GeomEntity CLASS_F(1, 2);
    const BDS_GeomEntity CLASS_EXTERIOR(1, 3);

    BDS_Mesh m;

    std::map<Ptr<BDS_Point>, Ptr<MeshVertexAbstract>, PointLessThan> recoverMap;
    std::map<Ptr<MeshVertexAbstract>, Ptr<BDS_Point>> recoverMapInv;
    // std::vector<GEdge *> edges = replacementEdges ? *replacementEdges : gf->edges();

    auto & geom_surface = surface->geom_surface();
    std::vector<Ptr<BDS_Point>> points(all_vertices.size());
    int count = 0;
    for (auto & vtx : all_vertices) {
        auto & ge = vtx->geom_shape();
        auto param = reparam_mesh_vertex_on_surface(vtx, geom_surface);
        auto pp = m.add_point(count, param, &geom_surface);
        auto g = m.add_geom(ge.id(), ge.dim());
        pp->g_ = g;
        recoverMap[pp] = vtx;
        recoverMapInv[vtx] = pp;
        points[count] = pp;
        count++;
    }

    build_bds_mesh(surface, m, all_vertices, recoverMapInv);

    // Recover the boundary edges and compute characteristic lenghts using mesh
    // edge spacing. If two of these edges intersect, then the 1D mesh have to be
    // densified
    Log::debug("Recovering {} model curves", curves.size());
    std::set<EdgeToRecover> edgesToRecover;
    std::set<EdgeToRecover> edgesNotRecovered;
    for (auto & crv : curves) {
        if (crv->is_mesh_degenerated())
            continue;
        recoverEdge(m, surface, crv, recoverMapInv, &edgesToRecover, &edgesNotRecovered, 1);
    }
    for (auto & crv : surface->embedded_curves()) {
        if (crv->is_mesh_degenerated())
            continue;
        recoverEdge(m, surface, crv, recoverMapInv, &edgesToRecover, &edgesNotRecovered, 1);
    }

    // effectively recover the medge
    for (auto & crv : curves) {
        if (crv->is_mesh_degenerated())
            continue;

        if (not recoverEdge(m,
                            surface,
                            crv,
                            recoverMapInv,
                            &edgesToRecover,
                            &edgesNotRecovered,
                            2)) {
            return false;
        }
    }
    Log::debug("Recovering {} mesh edges ({} not recovered)",
               edgesToRecover.size(),
               edgesNotRecovered.size());
    if (edgesNotRecovered.size()) {
        // TODO
        throw Exception("Not implemented yet");
    }
    Log::debug("Boundary edges recovered for surface {}", surface->id());

    // ---

    // look for a triangle that has a negative node and recursively tag all
    // exterior triangles
    for (auto & tri : m.triangles())
        tri->g_ = std::nullopt;
    for (auto & tri : m.triangles()) {
        auto res = tri->get_nodes();
        if (res.has_value()) {
            auto n = res.value();
            if (n[0]->id() < 0 || n[1]->id() < 0 || n[2]->id() < 0) {
                recur_tag(tri.get(), CLASS_EXTERIOR);
                break;
            }
        }
    }

    // now find an edge that has belongs to one of the exterior triangles
    for (auto & e : m.edges()) {
        if (e->g_.has_value() && e->num_faces() == 2) {
            auto faces = e->faces();
            if (faces[0]->g_.value() == CLASS_EXTERIOR) {
                recur_tag(faces[1], CLASS_F);
                break;
            }
            else if (faces[1]->g_.value() == CLASS_EXTERIOR) {
                recur_tag(faces[0], CLASS_F);
                break;
            }
        }
    }
    for (auto & tri : m.triangles()) {
        if (tri->g_.value() == CLASS_EXTERIOR)
            tri->g_ = std::nullopt;
    }

    for (auto & e : m.edges()) {
        if (e->g_.has_value() && e->num_faces() == 2) {
            auto faces = e->faces();
            auto oface = e->opposite_of();
            if (oface[0]->id() < 0) {
                recur_tag(faces[1], CLASS_F);
                break;
            }
            else if (oface[1]->id() < 0) {
                recur_tag(faces[0], CLASS_F);
                break;
            }
        }
    }

    // ---

    for (auto & edge : surface->embedded_curves()) {
        if (edge->is_mesh_degenerated())
            continue;
        recoverEdge(m, surface, edge, recoverMapInv, &edgesToRecover, &edgesNotRecovered, 2);
    }

    // ---

    // compute characteristic lengths at vertices
    if (!only_initial_mesh) {
        Log::debug("Computing mesh size field at mesh nodes {}", edgesToRecover.size());
        for (auto & [id, pp] : m.points()) {
            if (auto itv = recoverMap.find(pp); itv != recoverMap.end()) {
                auto vtx = itv->second;
                auto & ge = vtx->geom_shape();
                double lc;
                if (ge.dim() == 0) {
                    lc = 0.1; // BGM_MeshSize(ge, 0, 0, here->x(), here->y(), here->z());
                }
                else if (ge.dim() == 1) {
                    // auto u = here->parameter();
                    lc = 0.1; // BGM_MeshSize(ge, u, 0, here->x(), here->y(), here->z());
                }
                else
                    lc = MAX_LC;
                pp->set_lc(lc);
            }
        }
    }

    // ---

    // delete useless stuff
    for (auto & tri : m.triangles()) {
        if (!tri->g_)
            m.del_face(tri.get());
    }
    m.cleanup();

    for (auto & e : m.edges()) {
        if (e->num_faces() == 0)
            m.del_edge(e.get());
        else {
            if (not e->g_.has_value())
                e->g_ = CLASS_F;
            if (not e->p1_->g_.has_value() || e->p1_->g_->degree > e->g_->degree)
                e->p1_->g_ = e->g_;
            if (not e->p2_->g_.has_value() || e->p2_->g_->degree > e->g_->degree)
                e->p2_->g_ = e->g_;
        }
    }
    m.cleanup();
    m.del_point(m.find_point(-1).value());
    m.del_point(m.find_point(-2).value());
    m.del_point(m.find_point(-3).value());
    m.del_point(m.find_point(-4).value());

    // ---

    for (auto & t : m.triangles()) {
        if (t->active()) {
            auto res = t->get_nodes();
            if (res.has_value()) {
                auto n = res.value();
                auto v1 = recoverMap[n[0]];
                auto v2 = recoverMap[n[1]];
                auto v3 = recoverMap[n[2]];
                if (v1 != v2 && v1 != v3 && v2 != v3)
                    surface->add_triangle({ v1, v2, v3 });
            }
        }
    }

    // ---

    Log::debug("Delaunizing the initial mesh");
    delaunayize_bds(surface, m);

    // ---

    surface->delete_mesh();

    // ---

    BDS2GMSH(m, surface, recoverMap);

    insertion_algo(surface);

    delete_unused_vertices(surface);

    return true;
}

void
SchemeDelaunay::mesh_surface(Ptr<MeshSurface> surface)
{
    mesh_generation(surface, surface->curves(), false);
}

} // namespace krado
