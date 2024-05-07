#pragma once

// Rewrite of BDS.h from gmsh

// This is a 2D version of the Bidirectional Data Structure (BDS) of Shephard and Beall
// Points may know the normals to the surface they are classified on default values are 0,0,1

#include "krado/exception.h"
#include "krado/vector.h"
#include "krado/uv_param.h"
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>

namespace krado {

class BDS_Edge;
class BDS_Face;
class BDS_Mesh;
class BDS_Point;
class BDS_Vector;

class GeomVertex;
class GeomCurve;
class GeomSurface;

class BDS_GeomEntity {
public:
    int classif_tag;
    int classif_degree;

    BDS_GeomEntity(int a, int b);

    ~BDS_GeomEntity() = default;

    bool operator<(const BDS_GeomEntity & other) const;
};

class BDS_Point {
    // the first size is the one dictated by the Background Mesh the
    // second one is dictated by characteristic lengths at points and is
    // propagated
    double _lcBGM, _lcPTS;

public:
    double x, y, z;
    double u, v;
    bool config_modified;
    short degenerated;
    BDS_Point * periodic_counterpart;
    int iD;
    BDS_GeomEntity * g;
    std::vector<BDS_Edge *> edges;

    double & lcBGM();

    double & lc();

    double const & lc() const;

    bool operator<(const BDS_Point & other) const;

    void del(BDS_Edge * e);

    std::vector<BDS_Face *> get_triangles() const;

    explicit BDS_Point(int id, double x = 0, double y = 0, double z = 0);
};

class BDS_Edge {
    std::vector<BDS_Face *> _faces;

public:
    BDS_Edge(BDS_Point * A, BDS_Point * B);

    BDS_Face * faces(std::size_t const i) const;

    double length() const;

    int num_faces() const;

    int num_triangles() const;

    BDS_Point * common_vertex(const BDS_Edge * other) const;

    BDS_Point * other_vertex(const BDS_Point * p) const;

    void add_face(BDS_Face * f);

    bool operator<(const BDS_Edge & other) const;

    BDS_Face * other_face(const BDS_Face * f) const;

    void del(BDS_Face * t);

    void opposite_of(BDS_Point * oface[2]) const;

    void compute_neighborhood(BDS_Point * t1[4], BDS_Point * t2[4], BDS_Point * oface[2]) const;

public:
    bool deleted;
    BDS_Point *p1, *p2;
    BDS_GeomEntity * g;
};

class BDS_Face {
public:
    BDS_Face(BDS_Edge * A, BDS_Edge * B, BDS_Edge * C, BDS_Edge * D = nullptr);

    int num_edges() const;

    BDS_Edge * opposite_edge(BDS_Point * p) const;

    BDS_Point * opposite_vertex(BDS_Edge * e) const;

    bool get_nodes(BDS_Point * n[4]) const;

public:
    bool deleted;
    BDS_Edge *e1, *e2, *e3, *e4;
    BDS_GeomEntity * g;
};

struct GeomLessThan {
    bool operator()(const BDS_GeomEntity * ent1, const BDS_GeomEntity * ent2) const;
};

struct PointLessThan {
    bool operator()(const BDS_Point * ent1, const BDS_Point * ent2) const;
};

struct PointLessThanLexicographic {
    static double t;
    bool operator()(const BDS_Point * ent1, const BDS_Point * ent2) const;
};

struct EdgeLessThan {
    bool operator()(const BDS_Edge * ent1, const BDS_Edge * ent2) const;
};

class BDS_SwapEdgeTest {
public:
    virtual bool
    operator()(BDS_Point * p1, BDS_Point * p2, BDS_Point * q1, BDS_Point * q2) const = 0;
    virtual bool operator()(BDS_Point * p1,
                            BDS_Point * p2,
                            BDS_Point * p3,
                            BDS_Point * q1,
                            BDS_Point * q2,
                            BDS_Point * q3,
                            BDS_Point * op1,
                            BDS_Point * op2,
                            BDS_Point * op3,
                            BDS_Point * oq1,
                            BDS_Point * oq2,
                            BDS_Point * oq3) const = 0;
    virtual ~BDS_SwapEdgeTest() = default;
};

class BDS_SwapEdgeTestRecover : public BDS_SwapEdgeTest {
public:
    BDS_SwapEdgeTestRecover() = default;
    bool operator()(BDS_Point * p1, BDS_Point * p2, BDS_Point * q1, BDS_Point * q2) const override;
    bool operator()(BDS_Point * p1,
                    BDS_Point * p2,
                    BDS_Point * p3,
                    BDS_Point * q1,
                    BDS_Point * q2,
                    BDS_Point * q3,
                    BDS_Point * op1,
                    BDS_Point * op2,
                    BDS_Point * op3,
                    BDS_Point * oq1,
                    BDS_Point * oq2,
                    BDS_Point * oq3) const override;
    ~BDS_SwapEdgeTestRecover() override = default;
};

class BDS_SwapEdgeTestQuality : public BDS_SwapEdgeTest {
    bool test_quality, test_small_triangles;

public:
    explicit BDS_SwapEdgeTestQuality(bool a, bool b = true) :
        test_quality(a),
        test_small_triangles(b)
    {
    }
    bool operator()(BDS_Point * p1, BDS_Point * p2, BDS_Point * q1, BDS_Point * q2) const override;
    bool operator()(BDS_Point * p1,
                    BDS_Point * p2,
                    BDS_Point * p3,
                    BDS_Point * q1,
                    BDS_Point * q2,
                    BDS_Point * q3,
                    BDS_Point * op1,
                    BDS_Point * op2,
                    BDS_Point * op3,
                    BDS_Point * oq1,
                    BDS_Point * oq2,
                    BDS_Point * oq3) const override;
    ~BDS_SwapEdgeTestQuality() override = default;
};

class BDS_SwapEdgeTestNormals : public BDS_SwapEdgeTest {
    const GeomSurface & gf;
    double _ori;

public:
    BDS_SwapEdgeTestNormals(const GeomSurface & gf, double ori) : gf(gf), _ori(ori) {}
    bool operator()(BDS_Point * p1, BDS_Point * p2, BDS_Point * q1, BDS_Point * q2) const override;
    bool operator()(BDS_Point * p1,
                    BDS_Point * p2,
                    BDS_Point * p3,
                    BDS_Point * q1,
                    BDS_Point * q2,
                    BDS_Point * q3,
                    BDS_Point * op1,
                    BDS_Point * op2,
                    BDS_Point * op3,
                    BDS_Point * oq1,
                    BDS_Point * oq2,
                    BDS_Point * oq3) const override;
};

struct EdgeToRecover {
    int p1, p2;
    const GeomCurve * ge;

    EdgeToRecover(int p1, int p2, const GeomCurve * ge);
    bool operator<(const EdgeToRecover & other) const;
};

class BDS_Mesh {
public:
    explicit BDS_Mesh(int MAXX = 0) : MAXPOINTNUMBER(MAXX) {}
    virtual ~BDS_Mesh();
    BDS_Mesh(const BDS_Mesh & other);
    // Points
    BDS_Point * add_point(int num, double x, double y, double z);
    BDS_Point * add_point(int num, UVParam uv, const GeomSurface & gf);
    void del_point(BDS_Point * p);
    BDS_Point * find_point(int num);
    // Edges
    BDS_Edge * add_edge(int p1, int p2);
    void del_edge(BDS_Edge * e);
    BDS_Edge * find_edge(int p1, int p2);
    BDS_Edge * find_edge(BDS_Point * p1, BDS_Point * p2);
    BDS_Edge * find_edge(BDS_Point * p1, int p2);
    BDS_Edge * find_edge(BDS_Point * p1, BDS_Point * p2, BDS_Face * t) const;
    // Triangles
    BDS_Face * add_triangle(int p1, int p2, int p3);
    BDS_Face * add_triangle(BDS_Edge * e1, BDS_Edge * e2, BDS_Edge * e3);
    void del_face(BDS_Face * t);
    BDS_Face * find_triangle(BDS_Edge * e1, BDS_Edge * e2, BDS_Edge * e3);
    // Geom entities
    void add_geom(int degree, int tag);
    BDS_GeomEntity * get_geom(int p1, int p2);
    // 2D operators
    std::tuple<BDS_Edge *, bool> recover_edge(int p1,
                                              int p2,
                                              std::set<EdgeToRecover> & e2r,
                                              std::set<EdgeToRecover> & not_recovered);
    BDS_Edge * recover_edge_fast(BDS_Point * p1, BDS_Point * p2);

    /// Can invalidate the iterators for \p edge
    bool swap_edge(BDS_Edge *, const BDS_SwapEdgeTest & theTest, bool force = false);
    bool collapse_edge_parametric(BDS_Edge *, BDS_Point *, bool = false);
    bool smooth_point_centroid(BDS_Point * p, const GeomSurface & gf, double thresh);
    bool split_edge(BDS_Edge *, BDS_Point *, bool check_area_param = false);
    bool edge_constraint(BDS_Point * p1, BDS_Point * p2);
    // Global operators
    void cleanup();

    int MAXPOINTNUMBER;
    double Min[3], Max[3], LC;
    std::set<BDS_GeomEntity *, GeomLessThan> geom;
    std::set<BDS_Point *, PointLessThan> points;
    std::vector<BDS_Edge *> edges;
    std::vector<BDS_Face *> triangles;
};

Vector normal_triangle(BDS_Point * p1, BDS_Point * p2, BDS_Point * p3);

void recur_tag(BDS_Face * t, BDS_GeomEntity * g);

bool intersect_edges_2d(double x1,
                        double y1,
                        double x2,
                        double y2,
                        double x3,
                        double y3,
                        double x4,
                        double y4,
                        double x[2]);
double bds_face_validity(GeomSurface * gf, BDS_Face * f);

} // namespace krado
