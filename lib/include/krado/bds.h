// SPDX-FileCopyrightText: 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include "krado/types.h"
#include "krado/point.h"
#include "krado/uv_param.h"
#include "krado/qtr.h"
#include "krado/ptr.h"
#include <vector>
#include <map>
#include <set>

namespace krado {

class BDS_Edge;
class BDS_Face;
class GeomSurface;
class GeomCurve;

class BDS_GeomEntity {
public:
    BDS_GeomEntity(i32 tag, i32 degree);

    bool operator<(const BDS_GeomEntity & other) const;
    bool operator==(const BDS_GeomEntity & other) const;

    i32 tag;
    i32 degree;
};

class BDS_Point {
public:
    BDS_Point(i32 id, Point pt, UVParam uv);

    i32 id() const;
    double lc() const;
    void set_lc(double lc);
    Point point() const;
    UVParam uv() const;
    double u() const;
    double v() const;
    u8 degenerated() const;
    void del(Ptr<BDS_Edge> e);
    std::vector<BDS_Face *> triangles() const;
    bool config_modified() const;

    bool operator<(const BDS_Point & other) const;

private:
    // Characteristic length at point and is propagated
    double lc_pts_;
    Point pt_;
    UVParam uv_;
    bool config_modified_;
    u8 degenerated_;
    i32 id_;
    Ptr<BDS_Point> periodic_counterpart_;

public:
    Optional<BDS_GeomEntity> g_;

private:
    std::vector<Ptr<BDS_Edge>> edges_;

    friend class BDS_Edge;
    friend class BDS_Mesh;
    friend class BDS_SwapEdgeTestQuality;
};

class BDS_Edge {
public:
    BDS_Edge(Ptr<BDS_Point> A, Ptr<BDS_Point> B);
    std::vector<BDS_Face *> faces();
    double length() const;
    bool deleted() const;
    bool active() const;
    void del();
    int num_faces() const;
    int num_triangles() const;
    Ptr<BDS_Point> common_vertex(Ptr<const BDS_Edge> other) const;
    Ptr<BDS_Point> other_vertex(Ptr<const BDS_Point> p) const;
    void add_face(BDS_Face * f);
    bool operator<(const BDS_Edge & other) const;
    Optional<BDS_Face *> other_face(const BDS_Face * f) const;
    void del(BDS_Face * t);
    std::array<Ptr<BDS_Point>, 2> opposite_of() const;
    std::tuple<std::array<Ptr<BDS_Point>, 3>,
               std::array<Ptr<BDS_Point>, 3>,
               std::array<Ptr<BDS_Point>, 2>>
    compute_neighborhood() const;

private:
    Ptr<BDS_Point> opposite_vertex(const std::array<Ptr<BDS_Point>, 3> & pts) const;

    std::vector<BDS_Face *> faces_;
    bool deleted_;

public:
    Ptr<BDS_Point> p1_, p2_;
    Optional<BDS_GeomEntity> g_;

    friend class BDS_Face;
    friend class BDS_Mesh;
};

class BDS_Face {
public:
    BDS_Face(Ptr<BDS_Edge> A, Ptr<BDS_Edge> B, Ptr<BDS_Edge> C);
    bool deleted() const;
    bool active() const;
    int num_edges() const;
    Optional<Ptr<BDS_Edge>> opposite_edge(Ptr<BDS_Point> p);
    Optional<Ptr<BDS_Point>> opposite_vertex(Ptr<BDS_Edge> e);
    Optional<std::array<Ptr<BDS_Point>, 3>> get_nodes() const;

public:
    bool deleted_;
    Ptr<BDS_Edge> e1_, e2_, e3_;
    Optional<BDS_GeomEntity> g_;
};

struct GeomLessThan {
    bool
    operator()(const BDS_GeomEntity & ent1, const BDS_GeomEntity & ent2) const
    {
        return ent1 < ent2;
    }
};

struct PointLessThan {
    bool
    operator()(Ptr<const BDS_Point> ent1, Ptr<const BDS_Point> ent2) const
    {
        return *ent1 < *ent2;
    }
};

struct EdgeLessThan {
    bool
    operator()(Ptr<const BDS_Edge> ent1, Ptr<const BDS_Edge> ent2) const
    {
        return *ent1 < *ent2;
    }
};

class BDS_SwapEdgeTest {
public:
    virtual bool operator()(Ptr<const BDS_Point> p1,
                            Ptr<const BDS_Point> p2,
                            Ptr<const BDS_Point> q1,
                            Ptr<const BDS_Point> q2) const = 0;
    virtual bool operator()(Ptr<const BDS_Point> p1,
                            Ptr<const BDS_Point> p2,
                            Ptr<const BDS_Point> p3,
                            Ptr<const BDS_Point> q1,
                            Ptr<const BDS_Point> q2,
                            Ptr<const BDS_Point> q3,
                            Ptr<const BDS_Point> op1,
                            Ptr<const BDS_Point> op2,
                            Ptr<const BDS_Point> op3,
                            Ptr<const BDS_Point> oq1,
                            Ptr<const BDS_Point> oq2,
                            Ptr<const BDS_Point> oq3) const = 0;
    virtual ~BDS_SwapEdgeTest() = default;
};

class BDS_SwapEdgeTestRecover : public BDS_SwapEdgeTest {
public:
    BDS_SwapEdgeTestRecover();
    bool operator()(Ptr<const BDS_Point> p1,
                    Ptr<const BDS_Point> p2,
                    Ptr<const BDS_Point> q1,
                    Ptr<const BDS_Point> q2) const override;
    bool operator()(Ptr<const BDS_Point> p1,
                    Ptr<const BDS_Point> p2,
                    Ptr<const BDS_Point> p3,
                    Ptr<const BDS_Point> q1,
                    Ptr<const BDS_Point> q2,
                    Ptr<const BDS_Point> q3,
                    Ptr<const BDS_Point> op1,
                    Ptr<const BDS_Point> op2,
                    Ptr<const BDS_Point> op3,
                    Ptr<const BDS_Point> oq1,
                    Ptr<const BDS_Point> oq2,
                    Ptr<const BDS_Point> oq3) const override;
    virtual ~BDS_SwapEdgeTestRecover() {}
};

class BDS_SwapEdgeTestQuality : public BDS_SwapEdgeTest {
    bool test_quality_, test_small_triangles_;

public:
    BDS_SwapEdgeTestQuality(bool a, bool b = true);
    bool operator()(Ptr<const BDS_Point> p1,
                    Ptr<const BDS_Point> p2,
                    Ptr<const BDS_Point> q1,
                    Ptr<const BDS_Point> q2) const override;
    bool operator()(Ptr<const BDS_Point> p1,
                    Ptr<const BDS_Point> p2,
                    Ptr<const BDS_Point> p3,
                    Ptr<const BDS_Point> q1,
                    Ptr<const BDS_Point> q2,
                    Ptr<const BDS_Point> q3,
                    Ptr<const BDS_Point> op1,
                    Ptr<const BDS_Point> op2,
                    Ptr<const BDS_Point> op3,
                    Ptr<const BDS_Point> oq1,
                    Ptr<const BDS_Point> oq2,
                    Ptr<const BDS_Point> oq3) const override;
    virtual ~BDS_SwapEdgeTestQuality() {}
};

class BDS_SwapEdgeTestNormals : public BDS_SwapEdgeTest {
    GeomSurface * gf_;
    double ori_;

public:
    BDS_SwapEdgeTestNormals(GeomSurface * _gf, double ori);
    virtual bool operator()(Ptr<const BDS_Point> p1,
                            Ptr<const BDS_Point> p2,
                            Ptr<const BDS_Point> q1,
                            Ptr<const BDS_Point> q2) const;
    virtual bool operator()(Ptr<const BDS_Point> p1,
                            Ptr<const BDS_Point> p2,
                            Ptr<const BDS_Point> p3,
                            Ptr<const BDS_Point> q1,
                            Ptr<const BDS_Point> q2,
                            Ptr<const BDS_Point> q3,
                            Ptr<const BDS_Point> op1,
                            Ptr<const BDS_Point> op2,
                            Ptr<const BDS_Point> op3,
                            Ptr<const BDS_Point> oq1,
                            Ptr<const BDS_Point> oq2,
                            Ptr<const BDS_Point> oq3) const;
};

struct EdgeToRecover {
    EdgeToRecover(int p1, int p2, const GeomCurve * ge);
    bool operator<(const EdgeToRecover & other) const;

    const GeomCurve *
    geom_curve() const
    {
        return this->ge_;
    }

private:
    int p1_, p2_;
    const GeomCurve * ge_;
};

class BDS_Mesh {
public:
    BDS_Mesh(int max_pts = 0);

    const std::map<int, Ptr<BDS_Point>> & points() const;
    Span<const Ptr<BDS_Edge>> edges() const;
    Span<const Qtr<BDS_Face>> triangles() const;
    // Points
    Ptr<BDS_Point> add_point(int num, Point pt);
    Ptr<BDS_Point> add_point(int num, UVParam uv, const GeomSurface * gf);
    void del_point(Ptr<BDS_Point> p);
    Optional<Ptr<BDS_Point>> find_point(int num) const;
    // Edges
    Optional<Ptr<BDS_Edge>> add_edge(int p1, int p2);
    Ptr<BDS_Edge> add_edge(Ptr<BDS_Point> p1, Ptr<BDS_Point> p2);
    void del_edge(Ptr<BDS_Edge> e);
    Optional<Ptr<BDS_Edge>> find_edge(int p1, int p2) const;
    Optional<Ptr<BDS_Edge>> find_edge(Ptr<BDS_Point> p1, Ptr<BDS_Point> p2) const;
    Optional<Ptr<BDS_Edge>> find_edge(Ptr<BDS_Point> p1, int p2) const;
    Optional<Ptr<BDS_Edge>> find_edge(Ptr<BDS_Point> p1, Ptr<BDS_Point> p2, BDS_Face * t) const;
    // Triangles
    Optional<BDS_Face *> add_triangle(int p1, int p2, int p3);
    Optional<BDS_Face *> add_triangle(Ptr<BDS_Edge> e1, Ptr<BDS_Edge> e2, Ptr<BDS_Edge> e3);
    void del_face(BDS_Face * t);
    Optional<BDS_Face *> find_triangle(Ptr<BDS_Edge> e1, Ptr<BDS_Edge> e2, Ptr<BDS_Edge> e3) const;
    // Geom entities
    BDS_GeomEntity add_geom(int tag, int degree);
    // 2D operators
    Optional<Ptr<BDS_Edge>> recover_edge(int p1,
                                         int p2,
                                         bool & fatal,
                                         std::set<EdgeToRecover> * e2r = nullptr,
                                         std::set<EdgeToRecover> * not_recovered = nullptr);
    Optional<Ptr<BDS_Edge>> recover_edge_fast(Ptr<BDS_Point> p1, Ptr<BDS_Point> p2);

    /// Can invalidate the iterators for \p edge
    bool swap_edge(Ptr<BDS_Edge>, const BDS_SwapEdgeTest & theTest, bool force = false);
    bool collapse_edge_parametric(Ptr<BDS_Edge>, Ptr<BDS_Point>, bool = false);
    bool smooth_point_centroid(Ptr<BDS_Point> p, const GeomSurface & gf, double thresh);
    bool split_edge(Ptr<BDS_Edge>, Ptr<BDS_Point>, bool check_area_param = false);
    void cleanup();

private:
    int max_point_num_;
    std::set<BDS_GeomEntity, GeomLessThan> geom_;
    std::map<int, Ptr<BDS_Point>> points_;
    std::vector<Ptr<BDS_Edge>> edges_;
    std::vector<Qtr<BDS_Face>> triangles_;
};

void recur_tag(BDS_Face * t, BDS_GeomEntity g);

} // namespace krado
