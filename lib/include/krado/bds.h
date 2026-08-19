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
    BDS_Point(i32 id, Point pt);
    BDS_Point(i32 id, Point pt, UVParam uv, BDS_GeomEntity ge);

    [[nodiscard]] i32 id() const;
    [[nodiscard]] double lc() const;
    void set_lc(double lc);
    [[nodiscard]] Point point() const;
    [[nodiscard]] UVParam uv() const;
    [[nodiscard]] double u() const;
    [[nodiscard]] double v() const;
    [[nodiscard]] u8 degenerated() const;
    void del(WeakPtr<BDS_Edge> e);
    [[nodiscard]] std::vector<WeakPtr<BDS_Face>> triangles() const;
    [[nodiscard]] bool config_modified() const;

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
    std::vector<WeakPtr<BDS_Edge>> edges_;

    friend class BDS_Edge;
    friend class BDS_Mesh;
    friend class BDS_SwapEdgeTestQuality;
};

class BDS_Edge {
public:
    BDS_Edge(WeakPtr<BDS_Point> A,
             WeakPtr<BDS_Point> B,
             Optional<BDS_GeomEntity> ge = std::nullopt);
    std::vector<WeakPtr<BDS_Face>> faces();
    [[nodiscard]] double length() const;
    [[nodiscard]] bool deleted() const;
    [[nodiscard]] bool active() const;
    void del();
    [[nodiscard]] int num_faces() const;
    [[nodiscard]] int num_triangles() const;
    [[nodiscard]] WeakPtr<BDS_Point> common_vertex(WeakPtr<const BDS_Edge> other) const;
    [[nodiscard]] WeakPtr<BDS_Point> other_vertex(WeakPtr<const BDS_Point> p) const;
    void add_face(WeakPtr<BDS_Face> f);
    bool operator<(const BDS_Edge & other) const;
    [[nodiscard]] Optional<WeakPtr<BDS_Face>> other_face(WeakPtr<const BDS_Face> f) const;
    void del(WeakPtr<BDS_Face> t);
    [[nodiscard]] std::array<WeakPtr<BDS_Point>, 2> opposite_of() const;
    [[nodiscard]] std::tuple<std::array<WeakPtr<BDS_Point>, 3>,
                             std::array<WeakPtr<BDS_Point>, 3>,
                             std::array<WeakPtr<BDS_Point>, 2>>
    compute_neighborhood() const;

private:
    [[nodiscard]] WeakPtr<BDS_Point>
    opposite_vertex(const std::array<WeakPtr<BDS_Point>, 3> & pts) const;

    std::vector<WeakPtr<BDS_Face>> faces_;
    bool deleted_;

public:
    WeakPtr<BDS_Point> p1_, p2_;
    Optional<BDS_GeomEntity> g_;

    friend class BDS_Face;
    friend class BDS_Mesh;
};

class BDS_Face {
public:
    BDS_Face(WeakPtr<BDS_Edge> A, WeakPtr<BDS_Edge> B, WeakPtr<BDS_Edge> C);
    [[nodiscard]] bool deleted() const;
    [[nodiscard]] bool active() const;
    [[nodiscard]] int num_edges() const;
    Optional<WeakPtr<BDS_Edge>> opposite_edge(WeakPtr<BDS_Point> p);
    Optional<WeakPtr<BDS_Point>> opposite_vertex(WeakPtr<BDS_Edge> e);
    [[nodiscard]] Optional<std::array<WeakPtr<BDS_Point>, 3>> get_nodes() const;

public:
    bool deleted_;
    WeakPtr<BDS_Edge> e1_, e2_, e3_;
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
    operator()(WeakPtr<const BDS_Point> ent1, WeakPtr<const BDS_Point> ent2) const
    {
        return *ent1 < *ent2;
    }
};

struct EdgeLessThan {
    bool
    operator()(WeakPtr<const BDS_Edge> ent1, WeakPtr<const BDS_Edge> ent2) const
    {
        return *ent1 < *ent2;
    }
};

class BDS_SwapEdgeTest {
public:
    virtual bool operator()(WeakPtr<const BDS_Point> p1,
                            WeakPtr<const BDS_Point> p2,
                            WeakPtr<const BDS_Point> q1,
                            WeakPtr<const BDS_Point> q2) const = 0;
    virtual bool operator()(WeakPtr<const BDS_Point> p1,
                            WeakPtr<const BDS_Point> p2,
                            WeakPtr<const BDS_Point> p3,
                            WeakPtr<const BDS_Point> q1,
                            WeakPtr<const BDS_Point> q2,
                            WeakPtr<const BDS_Point> q3,
                            WeakPtr<const BDS_Point> op1,
                            WeakPtr<const BDS_Point> op2,
                            WeakPtr<const BDS_Point> op3,
                            WeakPtr<const BDS_Point> oq1,
                            WeakPtr<const BDS_Point> oq2,
                            WeakPtr<const BDS_Point> oq3) const = 0;
    virtual ~BDS_SwapEdgeTest() = default;
};

class BDS_SwapEdgeTestRecover : public BDS_SwapEdgeTest {
public:
    BDS_SwapEdgeTestRecover();
    bool operator()(WeakPtr<const BDS_Point> p1,
                    WeakPtr<const BDS_Point> p2,
                    WeakPtr<const BDS_Point> q1,
                    WeakPtr<const BDS_Point> q2) const override;
    bool operator()(WeakPtr<const BDS_Point> p1,
                    WeakPtr<const BDS_Point> p2,
                    WeakPtr<const BDS_Point> p3,
                    WeakPtr<const BDS_Point> q1,
                    WeakPtr<const BDS_Point> q2,
                    WeakPtr<const BDS_Point> q3,
                    WeakPtr<const BDS_Point> op1,
                    WeakPtr<const BDS_Point> op2,
                    WeakPtr<const BDS_Point> op3,
                    WeakPtr<const BDS_Point> oq1,
                    WeakPtr<const BDS_Point> oq2,
                    WeakPtr<const BDS_Point> oq3) const override;
};

class BDS_SwapEdgeTestQuality : public BDS_SwapEdgeTest {
    bool test_quality_, test_small_triangles_;

public:
    BDS_SwapEdgeTestQuality(bool a, bool b = true);
    bool operator()(WeakPtr<const BDS_Point> p1,
                    WeakPtr<const BDS_Point> p2,
                    WeakPtr<const BDS_Point> q1,
                    WeakPtr<const BDS_Point> q2) const override;
    bool operator()(WeakPtr<const BDS_Point> p1,
                    WeakPtr<const BDS_Point> p2,
                    WeakPtr<const BDS_Point> p3,
                    WeakPtr<const BDS_Point> q1,
                    WeakPtr<const BDS_Point> q2,
                    WeakPtr<const BDS_Point> q3,
                    WeakPtr<const BDS_Point> op1,
                    WeakPtr<const BDS_Point> op2,
                    WeakPtr<const BDS_Point> op3,
                    WeakPtr<const BDS_Point> oq1,
                    WeakPtr<const BDS_Point> oq2,
                    WeakPtr<const BDS_Point> oq3) const override;
};

class BDS_SwapEdgeTestNormals : public BDS_SwapEdgeTest {
    GeomSurface * gf_;
    double ori_;

public:
    BDS_SwapEdgeTestNormals(GeomSurface * _gf, double ori);
    bool operator()(WeakPtr<const BDS_Point> p1,
                    WeakPtr<const BDS_Point> p2,
                    WeakPtr<const BDS_Point> q1,
                    WeakPtr<const BDS_Point> q2) const override;
    bool operator()(WeakPtr<const BDS_Point> p1,
                    WeakPtr<const BDS_Point> p2,
                    WeakPtr<const BDS_Point> p3,
                    WeakPtr<const BDS_Point> q1,
                    WeakPtr<const BDS_Point> q2,
                    WeakPtr<const BDS_Point> q3,
                    WeakPtr<const BDS_Point> op1,
                    WeakPtr<const BDS_Point> op2,
                    WeakPtr<const BDS_Point> op3,
                    WeakPtr<const BDS_Point> oq1,
                    WeakPtr<const BDS_Point> oq2,
                    WeakPtr<const BDS_Point> oq3) const override;
};

struct EdgeToRecover {
    EdgeToRecover(int p1, int p2, const GeomCurve * ge);
    bool operator<(const EdgeToRecover & other) const;

    [[nodiscard]] const GeomCurve *
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

    [[nodiscard]] const std::map<int, Ptr<BDS_Point>> & points() const;
    [[nodiscard]] Span<const Ptr<BDS_Edge>> edges() const;
    [[nodiscard]] Span<const Ptr<BDS_Face>> triangles() const;
    // Points
    Ptr<BDS_Point> add_point(int num, Point pt);
    Ptr<BDS_Point> add_point(int num, UVParam uv, const GeomSurface * gf, BDS_GeomEntity ge);
    void del_point(Ptr<BDS_Point> p);
    [[nodiscard]] Optional<Ptr<BDS_Point>> find_point(int num) const;
    // Edges
    Optional<WeakPtr<BDS_Edge>> add_edge(int p1, int p2);
    Ptr<BDS_Edge> add_edge(Ptr<BDS_Point> p1, Ptr<BDS_Point> p2);
    void del_edge(WeakPtr<BDS_Edge> e);
    [[nodiscard]] Optional<WeakPtr<BDS_Edge>> find_edge(int p1, int p2) const;
    [[nodiscard]] Optional<WeakPtr<BDS_Edge>> find_edge(WeakPtr<BDS_Point> p1,
                                                        WeakPtr<BDS_Point> p2) const;
    [[nodiscard]] Optional<WeakPtr<BDS_Edge>> find_edge(WeakPtr<BDS_Point> p1, int p2) const;
    [[nodiscard]] Optional<WeakPtr<BDS_Edge>>
    find_edge(WeakPtr<BDS_Point> p1, WeakPtr<BDS_Point> p2, WeakPtr<BDS_Face> t) const;
    // Triangles
    Optional<Ptr<BDS_Face>>
    add_triangle(int p1, int p2, int p3, Optional<BDS_GeomEntity> ge = std::nullopt);
    Optional<Ptr<BDS_Face>> add_triangle(WeakPtr<BDS_Edge> e1,
                                         WeakPtr<BDS_Edge> e2,
                                         WeakPtr<BDS_Edge> e3,
                                         Optional<BDS_GeomEntity> ge = std::nullopt);
    void del_face(WeakPtr<BDS_Face> t);
    [[nodiscard]] Optional<WeakPtr<BDS_Face>>
    find_triangle(WeakPtr<BDS_Edge> e1, WeakPtr<BDS_Edge> e2, WeakPtr<BDS_Edge> e3) const;
    // Geom entities
    BDS_GeomEntity add_geom(int tag, int degree);
    // 2D operators
    Optional<WeakPtr<BDS_Edge>> recover_edge(int p1,
                                             int p2,
                                             bool & fatal,
                                             std::set<EdgeToRecover> * e2r = nullptr,
                                             std::set<EdgeToRecover> * not_recovered = nullptr);
    Optional<WeakPtr<BDS_Edge>> recover_edge_fast(WeakPtr<BDS_Point> p1, WeakPtr<BDS_Point> p2);

    /// Can invalidate the iterators for \p edge
    bool swap_edge(WeakPtr<BDS_Edge>, const BDS_SwapEdgeTest & theTest, bool force = false);
    bool collapse_edge_parametric(WeakPtr<BDS_Edge>, WeakPtr<BDS_Point>, bool = false);
    bool smooth_point_centroid(WeakPtr<BDS_Point> p, const GeomSurface & gf, double thresh);
    bool split_edge(WeakPtr<BDS_Edge>, WeakPtr<BDS_Point>, bool check_area_param = false);
    void cleanup();

private:
    int max_point_num_;
    std::set<BDS_GeomEntity, GeomLessThan> geom_;
    std::map<int, Ptr<BDS_Point>> points_;
    std::vector<Ptr<BDS_Edge>> edges_;
    std::vector<Ptr<BDS_Face>> triangles_;
};

void recur_tag(WeakPtr<BDS_Face> t, BDS_GeomEntity g);

} // namespace krado
