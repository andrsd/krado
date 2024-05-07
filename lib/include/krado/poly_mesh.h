#pragma once

// Rewrite of PolyMesh from gmsh

#include "krado/vector.h"
#include <vector>
#include <algorithm>
#include <stack>

namespace krado {

class PolyMesh {
public:
    class HalfEdge;
    class Face;

    class Vertex {
    public:
        Vertex(double x, double y, double z, int d = -1);

        Vector position;
        /// one incident half edge
        HalfEdge * he;
        int data;
    };

    class HalfEdge {
    public:
        explicit HalfEdge(Vertex * vv);

        Vector d() const;

        /// origin
        Vertex * v;
        /// incident face
        Face * f;
        /// previous half edge on the face
        HalfEdge * prev;
        /// next half edge on the face
        HalfEdge * next;
        /// opposite half edge (twin)
        HalfEdge * opposite;
        int data;
    };

    class Face {
    public:
        explicit Face(HalfEdge * e);

        // one half edge of the face
        HalfEdge * he;
        int data;
    };

    PolyMesh() = default;
    ~PolyMesh();

    void reset();

    /// compute the degree of a given vertex v
    size_t degree(const Vertex * v) const;

    size_t num_sides(const HalfEdge * he) const;

    // compute the normal of an internal vertex v
    Vector normal(const Vertex * v) const;

    HalfEdge * get_edge(Vertex * v0, Vertex * v1);

    void create_face(Face * f,
                     Vertex * v0,
                     Vertex * v1,
                     Vertex * v2,
                     HalfEdge * he0,
                     HalfEdge * he1,
                     HalfEdge * he2);

    // swap without asking questions
    //
    //         he1
    // v2  +------>-----+ v3
    //     | \          |
    //     |   \ he0    | he2
    // heo2|     \      |
    //     |  heo0 \    |
    //     |         \  |
    // v1  +----<-------+ v0
    //          heo1
    //
    //           he1
    //     +------------+
    //     |         /  |
    //     |   he0 /    | he2
    // heo2|    /       |
    //     |  /heo0     |
    //     |/           |
    //     +------------+
    //          heo1
    //

    int swap_edge(HalfEdge * he0);

    int merge_faces(HalfEdge * he);

    void clean();

    int split_edge(HalfEdge * he0m, const Vector & position, int data);

    //
    // v0   he0
    // ------------------>------ v1
    // |                      /
    // |                   /
    // |      v         /
    // |he2          /
    // |          /  he1
    // |       /
    // |    /
    // |/
    // v2
    void initialize_rectangle(double xmin, double xmax, double ymin, double ymax);

    int split_triangle(int index,
                       double x,
                       double y,
                       double z,
                       Face * f,
                       int (*do_swap)(PolyMesh::HalfEdge *, void *) = nullptr,
                       void * data = nullptr,
                       std::vector<HalfEdge *> * t = nullptr);

    std::vector<Vertex *> vertices;
    std::vector<HalfEdge *> hedges;
    std::vector<Face *> faces;
    std::vector<Vector> high_order_nodes;

private:
    void cleanv();
    void cleanh();
    void cleanf();
};

struct HalfEdgePtrLessThan {
    bool
    operator()(PolyMesh::HalfEdge * l1, PolyMesh::HalfEdge * l2) const
    {
        PolyMesh::Vertex * l10 = std::min(l1->v, l1->next->v);
        PolyMesh::Vertex * l11 = std::max(l1->v, l1->next->v);
        PolyMesh::Vertex * l20 = std::min(l2->v, l2->next->v);
        PolyMesh::Vertex * l21 = std::max(l2->v, l2->next->v);
        if (l10 < l20)
            return true;
        if (l10 > l20)
            return false;
        if (l11 > l21)
            return true;
        return false;
    }
};

struct HalfEdgePtrEqual {
    bool
    operator()(PolyMesh::HalfEdge * l1, PolyMesh::HalfEdge * l2) const
    {
        PolyMesh::Vertex * l10 = std::min(l1->v, l1->next->v);
        PolyMesh::Vertex * l11 = std::max(l1->v, l1->next->v);
        PolyMesh::Vertex * l20 = std::min(l2->v, l2->next->v);
        PolyMesh::Vertex * l21 = std::max(l2->v, l2->next->v);
        if (l10 == l20 && l11 == l21)
            return true;
        return false;
    }
};

} // namespace krado
