#include "krado/mesh_triangulation.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/bounding_box_3d.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/uv_param.h"
#include "krado/predicates.h"
#include "krado/log.h"
#include <cassert>
#include <unordered_map>
#include <list>
#include <set>

namespace krado {

struct NodeCopies {
    MeshVertexAbstract * mv;
    std::size_t n_copies;
    UVParam par[8]; // max 8 copies -- reduced to 4
    std::size_t id[8];

    NodeCopies(MeshVertexAbstract * mv, UVParam par) : mv(mv), n_copies(1) { this->par[0] = par; }

    void
    add_copy(UVParam p)
    {
        for (std::size_t i = 0; i < this->n_copies; ++i) {
            if (std::abs(this->par[i].u - p.u) < 1.e-9 && std::abs(this->par[i].v - p.v) < 1.e-9)
                return;
        }
        this->par[n_copies] = p;
        this->n_copies++;
    }

    size_t
    closest(UVParam p)
    {
        double minD = 1.e22;
        std::size_t I = 0;
        for (std::size_t i = 0; i < this->n_copies; ++i) {
            auto dist = utils::distance(p, this->par[i]);
            if (dist < minD) {
                minD = dist;
                I = i;
            }
        }
        return this->id[I];
    }
};

size_t
hilbert_coordinates(double x,
                    double y,
                    double x0,
                    double y0,
                    double xRed,
                    double yRed,
                    double xBlue,
                    double yBlue)
{
    size_t BIG = 1073741824;
    size_t RESULT = 0;
    for (int i = 0; i < 16; i++) {
        double coordRed = (x - x0) * xRed + (y - y0) * yRed;
        double coordBlue = (x - x0) * xBlue + (y - y0) * yBlue;
        xRed /= 2;
        yRed /= 2;
        xBlue /= 2;
        yBlue /= 2;
        if (coordRed <= 0 && coordBlue <= 0) { // quadrant 0
            x0 -= (xBlue + xRed);
            y0 -= (yBlue + yRed);
            std::swap(xRed, xBlue);
            std::swap(yRed, yBlue);
        }
        else if (coordRed <= 0 && coordBlue >= 0) { // quadrant 1
            RESULT += BIG;
            x0 += (xBlue - xRed);
            y0 += (yBlue - yRed);
        }
        else if (coordRed >= 0 && coordBlue >= 0) { // quadrant 2
            RESULT += 2 * BIG;
            x0 += (xBlue + xRed);
            y0 += (yBlue + yRed);
        }
        else if (coordRed >= 0 && coordBlue <= 0) { // quadrant 3
            x0 += (-xBlue + xRed);
            y0 += (-yBlue + yRed);
            std::swap(xRed, xBlue);
            std::swap(yRed, yBlue);
            xBlue = -xBlue;
            yBlue = -yBlue;
            xRed = -xRed;
            yRed = -yRed;
            RESULT += 3 * BIG;
        }
        else
            Log::warn("Hilbert failed {} {}", coordRed, coordBlue);
        BIG /= 4;
    }
    return RESULT;
}

static PolyMesh::Face *
Walk(PolyMesh::Face * f, double x, double y)
{
    assert(f != nullptr);

    Point POS(x, y, 0.);
    PolyMesh::HalfEdge * he = f->he;

    while (true) {
        PolyMesh::Vertex * v0 = he->v;
        PolyMesh::Vertex * v1 = he->next->v;
        PolyMesh::Vertex * v2 = he->next->next->v;

        double s0 = -orient2d(v0->position, v1->position, POS);
        double s1 = -orient2d(v1->position, v2->position, POS);
        double s2 = -orient2d(v2->position, v0->position, POS);

        if (s0 >= 0 && s1 >= 0 && s2 >= 0) {
            return he->f;
        }
        else if (s0 <= 0 && s1 >= 0 && s2 >= 0)
            he = he->opposite;
        else if (s1 <= 0 && s0 >= 0 && s2 >= 0)
            he = he->next->opposite;
        else if (s2 <= 0 && s0 >= 0 && s1 >= 0)
            he = he->next->next->opposite;
        else if (s0 <= 0 && s1 <= 0)
            he = s0 > s1 ? he->opposite : he->next->opposite;
        else if (s0 <= 0 && s2 <= 0)
            he = s0 > s2 ? he->opposite : he->next->next->opposite;
        else if (s1 <= 0 && s2 <= 0)
            he = s1 > s2 ? he->next->opposite : he->next->next->opposite;
        else {
            throw Exception("Could not find half-edge in walk for point {} {} on face {} {} {} / "
                            "{} {} {} / {} {} {} (orientation tests {} {} {})",
                            x,
                            y,
                            v0->position.x,
                            v0->position.y,
                            v0->position.z,
                            v1->position.x,
                            v1->position.y,
                            v1->position.z,
                            v2->position.x,
                            v2->position.y,
                            v2->position.z,
                            s0,
                            s1,
                            s2);
        }
        if (he == nullptr)
            break;
    }
    // should only come here wether the triangulated domain is not convex
    return nullptr;
}

// recover an edge that goes from v_start --> v_end
// ----------------------------------- assume it's internal !!!

static int
intersect(PolyMesh::Vertex * v0,
          PolyMesh::Vertex * v1,
          PolyMesh::Vertex * b0,
          PolyMesh::Vertex * b1)
{
    assert(v0 != nullptr);
    assert(v1 != nullptr);
    assert(b0 != nullptr);
    assert(b1 != nullptr);

    auto s0 = orient2d(v0->position, v1->position, b0->position);
    auto s1 = orient2d(v0->position, v1->position, b1->position);
    if (s0 * s1 >= 0)
        return 0;
    auto t0 = orient2d(b0->position, b1->position, v0->position);
    auto t1 = orient2d(b0->position, b1->position, v1->position);
    if (t0 * t1 >= 0)
        return 0;
    return 1;
}

static int
recover_edge(PolyMesh & pm, PolyMesh::Vertex * v_start, PolyMesh::Vertex * v_end)
{
    assert(v_start != nullptr);
    assert(v_end != nullptr);

    PolyMesh::HalfEdge * he = v_start->he;
    std::list<PolyMesh::HalfEdge *> list;

    assert(he != nullptr);
    do {
        PolyMesh::Vertex * v1 = he->next->v;
        if (v1 == v_end) {
            return 0; // edge exists
        }
        PolyMesh::Vertex * v2 = he->next->next->v;
        if (v2 == v_end) {
            return 0; // edge exists
        }

        if (intersect(v_start, v_end, v1, v2)) {
            list.push_back(he->next);
            break;
        }
        he = he->next->next->opposite;
    } while (he != v_start->he);

    if (list.empty())
        return -1;

    // find all intersections
    while (true) {
        he = list.back();
        he = he->opposite;
        if (!he)
            return -2;
        he = he->next;
        PolyMesh::Vertex * v1 = he->v;
        PolyMesh::Vertex * v2 = he->next->v;
        if (v2 == v_end) {
            break;
        }
        if (intersect(v_start, v_end, v1, v2)) {
            list.push_back(he);
        }
        else {
            he = he->next;
            v1 = he->v;
            v2 = he->next->v;
            if (v2 == v_end) {
                break;
            }
            if (intersect(v_start, v_end, v1, v2)) {
                list.push_back(he);
            }
            else {
                return -3;
            }
        }
    }

    int n_intersection = list.size();
    while (!list.empty()) {
        he = *list.begin();
        list.erase(list.begin());
        // ensure that swap is allowed (convex quad)
        if (intersect(he->v, he->next->v, he->next->next->v, he->opposite->next->next->v)) {
            // ensure that swap removes one intersection
            int still_intersect =
                intersect(v_start, v_end, he->next->next->v, he->opposite->next->next->v);
            pm.swap_edge(he);
            if (still_intersect)
                list.push_back(he);
        }
        else
            list.push_back(he);
    }
    return n_intersection;
}

static PolyMesh::HalfEdge *
Color(PolyMesh::HalfEdge * he, int color)
{
    assert(he != nullptr);

    std::stack<PolyMesh::Face *> stack;
    stack.push(he->f);

    PolyMesh::HalfEdge * other_side = nullptr;

    while (!stack.empty()) {
        PolyMesh::Face * f = stack.top();
        assert(f != nullptr);
        stack.pop();
        f->data = color;
        he = f->he;
        assert(he != nullptr);
        for (int i = 0; i < 3; i++) {
            if (he->data == -1 && he->opposite != nullptr && he->opposite->f->data == -1) {
                stack.push(he->opposite->f);
            }
            else if (he->data != -1 && he->opposite != nullptr) {
                other_side = he->opposite;
            }
            he = he->next;
        }
    }
    return other_side;
}

static int
delaunay_edge_criterion_plane_isotropic(PolyMesh::HalfEdge * he, void *)
{
    assert(he != nullptr);
    if (he->opposite == nullptr)
        return -1;
    auto * v0 = he->v;
    assert(he->next != nullptr);
    auto * v1 = he->next->v;
    assert(he->next->next != nullptr);
    auto * v2 = he->next->next->v;
    assert(he->opposite->next->next != nullptr);
    auto * v = he->opposite->next->next->v;

    // FIXME : should be oriented anyway !
    double result = -incircle(v0->position, v1->position, v2->position, v->position);

    return (result > 0) ? 1 : 0;
}

void
add_points(PolyMesh & pm, std::vector<double> & pts, BoundingBox3D & bb)
{
    const size_t N = pts.size() / 2;
    std::vector<double> X(N), Y(N);
    std::vector<size_t> HC(N), IND(N);
    PolyMesh::Face * f = pm.faces[0];
    for (size_t i = 0; i < N; i++) {
        X[i] = pts[2 * i];
        Y[i] = pts[2 * i + 1];
        HC[i] = hilbert_coordinates(X[i],
                                    Y[i],
                                    bb.center().x,
                                    bb.center().y,
                                    bb.max().x - bb.center().x,
                                    0,
                                    0,
                                    bb.max().y - bb.center().y);
        IND[i] = i;
    }
    std::sort(IND.begin(), IND.end(), [&](size_t i, size_t j) { return HC[i] < HC[j]; });

    for (size_t i = 0; i < N; i++) {
        size_t I = IND[i];
        f = Walk(f, X[I], Y[I]);
        pm.split_triangle(i, X[I], Y[I], 0, f, delaunay_edge_criterion_plane_isotropic, nullptr);
    }
}

static std::unordered_map<size_t, NodeCopies>
get_node_copies(MeshSurface & msurface)
{
    std::unordered_map<size_t, NodeCopies> copies;

    auto & gsurface = msurface.geom_surface();
    auto edges = msurface.curves();
    auto & emb_edges = msurface.embedded_curves();
    edges.insert(edges.end(), emb_edges.begin(), emb_edges.end());
    std::set<MeshCurve *> touched;

    if (edges.empty())
        // FIXME: edges.insert(edges.end(), gf->model()->firstEdge(), gf->model()->lastEdge());
        throw Exception("No edges");

    for (auto e : edges) {
        assert(e != nullptr);
        if (!e->is_mesh_degenerated()) {
            std::set<MeshVertexAbstract *, MeshVertexAbstract::PtrLessThan> e_vertices;
            for (auto & seg : e->segments()) {
                e_vertices.insert(seg.vertex(0));
                e_vertices.insert(seg.vertex(1));
            }
            int direction = -1;
            if (e->geom_curve().is_seam(gsurface)) {
                direction = 0;
                if (touched.find(e) == touched.end())
                    touched.insert(e);
                else
                    direction = 1;
            }
            // printf("model edge %lu %lu vertices\n", e->tag(), e_vertices.size());
            for (auto & vtx : e_vertices) {
                assert(vtx != nullptr);
                UVParam par;
                bool success;
                if (direction != -1) {
                    double t = 0;
                    if (vtx->geom_shape().dim() == 0)
                        std::tie(t, success) = reparam_mesh_vertex_on_curve(vtx, e->geom_curve());
                    else if (vtx->geom_shape().dim() == 1) {
                        auto cvtx = dynamic_cast<MeshCurveVertex *>(vtx);
                        t = cvtx->parameter();
                    }
                    else
                        throw Exception("A seam edge without CAD ?");
                    par = reparam_on_surface(gsurface, e->geom_curve(), t, direction);
                }
                // FIXME: when seams are implemented
                else {
                    // Hmm...
                    // if (!gsurface.have_parametrization() &&
                    //     gsurface.type() == GEntity::DiscreteSurface) {
                    //     par = UVParam(vtx->x(), vtx->y());
                    // }
                    // else
                    std::tie(par, success) = reparam_mesh_vertex_on_surface(vtx, gsurface);
                }
                std::unordered_map<size_t, NodeCopies>::iterator it = copies.find(vtx->num());
                if (it == copies.end()) {
                    NodeCopies c(vtx, par);
                    copies.insert(std::make_pair(vtx->num(), c));
                }
                else {
                    it->second.add_copy(par);
                }
            }
        }
    }

    auto emb_verts = msurface.embedded_vertices();
    for (auto & mvtx : emb_verts) {
        auto [param, success] = reparam_mesh_vertex_on_surface(mvtx, gsurface);
        NodeCopies c(mvtx, param);
        copies.insert(std::make_pair(mvtx->num(), c));
    }

    return copies;
}

PolyMesh
surface_initial_mesh(MeshSurface & msurface, bool recover, std::vector<double> * additional)
{
    auto faceTag = msurface.tag();

    PolyMesh pm;

    auto copies = get_node_copies(msurface);

    BoundingBox3D bb;
    for (auto & [gid, node] : copies) {
        for (size_t i = 0; i < node.n_copies; i++)
            bb += Point(node.par[i].u, node.par[i].v, 0);
    }
    bb *= 1.1;

    pm.initialize_rectangle(bb.min().x, bb.max().x, bb.min().y, bb.max().y);
    auto * f = pm.faces[0];
    for (auto & [num, node] : copies) {
        for (size_t i = 0; i < node.n_copies; ++i) {
            double x = node.par[i].u;
            double y = node.par[i].v;
            // find face in which lies x,y
            f = Walk(f, x, y);
            // split f and then swap edges to recover delaunayness
            pm.split_triangle(-1, x, y, 0, f, delaunay_edge_criterion_plane_isotropic, nullptr);
            // remember node tags
            node.id[i] = pm.vertices.size() - 1;
            pm.vertices[pm.vertices.size() - 1]->data = num;
        }
    }

    // pm->print4debug(faceTag);

    if (recover) {
        auto curves = msurface.curves();
        auto & emb_edges = msurface.embedded_curves();
        curves.insert(curves.end(), emb_edges.begin(), emb_edges.end());
        if (curves.empty())
            // FIXME: edges.insert(edges.end(), gf->model()->firstEdge(), gf->model()->lastEdge());
            throw Exception("No curves");

        for (auto & e : curves) {
            assert(e != nullptr);
            if (!e->is_mesh_degenerated()) {
                for (auto & s : e->segments()) {
                    auto c0 = copies.find(s.vertex(0)->num());
                    auto c1 = copies.find(s.vertex(1)->num());
                    if (c0 == copies.end() || c1 == copies.end())
                        throw Exception("Unable to find {} {} {} {}",
                                        s.vertex(0)->num(),
                                        s.vertex(1)->num(),
                                        c0 == copies.end(),
                                        c1 == copies.end());
                    if (c0->second.n_copies > c1->second.n_copies)
                        std::swap(c0, c1);

                    for (size_t j = 0; j < c0->second.n_copies; ++j) {
                        auto * v0 = pm.vertices[c0->second.id[j]];
                        auto * v1 = pm.vertices[c1->second.closest(c0->second.par[j])];
                        int result = recover_edge(pm, v0, v1);
                        if (result < 0) {
                            throw Exception("Impossible to recover edge {} {} (error tag {})",
                                            s.vertex(0)->num(),
                                            s.vertex(1)->num(),
                                            result);
                        }
                        else {
                            auto * he = pm.get_edge(v0, v1);
                            if (he) {
                                if (he->opposite)
                                    he->opposite->data = e->tag();
                                he->data = e->tag();
                            }
                        }
                    }
                }
            }
        }

        // color all PolyMesh::Faces
        // the first 4 vertices are "infinite vertices" --> color them with tag -2
        // meaning exterior
        auto * other_side = Color(pm.vertices[0]->he, -2);
        // other_side is inthernal to the face --> color them with tag faceTag
        other_side = Color(other_side, faceTag);
        // holes will be tagged -1

        // flip edges that have been scrambled
        int iter = 0;
        while (iter++ < 100) {
            int count = 0;
            for (auto he : pm.hedges) {
                assert(he != nullptr);
                assert(he->f != nullptr);
                if (he->opposite && he->f->data == faceTag && he->opposite->f->data == faceTag) {
                    if (delaunay_edge_criterion_plane_isotropic(he, nullptr)) {
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

    if (additional)
        add_points(pm, *additional, bb);

    return pm;
}

} // namespace krado
