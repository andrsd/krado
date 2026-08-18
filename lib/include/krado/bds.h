// SPDX-FileCopyrightText: 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include "krado/types.h"
#include "krado/point.h"
#include "krado/uv_param.h"
#include "krado/qtr.h"
#include "krado/ref.h"
#include "krado/vtr.h"
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
    void del(Ref<BDS_Edge> e);
    [[nodiscard]] std::vector<Vtr<BDS_Face>> triangles() const;
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
    Vtr<BDS_Point> periodic_counterpart_;

public:
    Optional<BDS_GeomEntity> g_;

private:
    std::vector<Ref<BDS_Edge>> edges_;

    friend class BDS_Edge;
    friend class BDS_Mesh;
    friend class BDS_SwapEdgeTestQuality;
};

class BDS_Edge {
public:
    BDS_Edge(Ref<BDS_Point> A, Ref<BDS_Point> B, Optional<BDS_GeomEntity> ge = std::nullopt);
    std::vector<Vtr<BDS_Face>> faces();
    [[nodiscard]] double length() const;
    [[nodiscard]] bool deleted() const;
    [[nodiscard]] bool active() const;
    void del();
    [[nodiscard]] int num_faces() const;
    [[nodiscard]] int num_triangles() const;
    [[nodiscard]] Optional<Ref<BDS_Point>> common_vertex(Ref<const BDS_Edge> other) const;
    [[nodiscard]] Vtr<BDS_Point> other_vertex(Ref<const BDS_Point> p) const;
    void add_face(Ref<BDS_Face> f);
    bool operator<(const BDS_Edge & other) const;
    [[nodiscard]] Optional<Ref<BDS_Face>> other_face(Ref<BDS_Face> f) const;
    void del(Vtr<BDS_Face> t);
    [[nodiscard]] std::array<Vtr<BDS_Point>, 2> opposite_of() const;
    [[nodiscard]] std::tuple<Optional<std::array<Ref<BDS_Point>, 3>>,
                             Optional<std::array<Ref<BDS_Point>, 3>>,
                             std::array<Optional<Ref<BDS_Point>>, 2>>
    compute_neighborhood() const;

private:
    [[nodiscard]] Ref<BDS_Point> opposite_vertex(const std::array<Ref<BDS_Point>, 3> & pts) const;

    std::vector<Vtr<BDS_Face>> faces_;
    bool deleted_;

public:
    Ref<BDS_Point> p1_, p2_;
    Optional<BDS_GeomEntity> g_;

    friend class BDS_Face;
    friend class BDS_Mesh;
};

class BDS_Face {
public:
    BDS_Face(Ref<BDS_Edge> A, Ref<BDS_Edge> B, Ref<BDS_Edge> C);
    [[nodiscard]] bool deleted() const;
    [[nodiscard]] bool active() const;
    [[nodiscard]] int num_edges() const;
    Optional<Ref<BDS_Edge>> opposite_edge(Ref<BDS_Point> p);
    Optional<Ref<BDS_Point>> opposite_vertex(Ref<BDS_Edge> e);
    [[nodiscard]] Optional<std::array<Ref<BDS_Point>, 3>> get_nodes() const;

public:
    bool deleted_;
    Ref<BDS_Edge> e1_, e2_, e3_;
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
    operator()(Ref<const BDS_Point> ent1, Ref<const BDS_Point> ent2) const
    {
        return *ent1 < *ent2;
    }
};

struct EdgeLessThan {
    bool
    operator()(Ref<const BDS_Edge> ent1, Ref<const BDS_Edge> ent2) const
    {
        return *ent1 < *ent2;
    }
};

class BDS_SwapEdgeTest {
public:
    virtual bool operator()(Ref<const BDS_Point> p1,
                            Ref<const BDS_Point> p2,
                            Ref<const BDS_Point> q1,
                            Ref<const BDS_Point> q2) const = 0;
    virtual bool operator()(Ref<const BDS_Point> p1,
                            Ref<const BDS_Point> p2,
                            Ref<const BDS_Point> p3,
                            Ref<const BDS_Point> q1,
                            Ref<const BDS_Point> q2,
                            Ref<const BDS_Point> q3,
                            Ref<const BDS_Point> op1,
                            Ref<const BDS_Point> op2,
                            Ref<const BDS_Point> op3,
                            Ref<const BDS_Point> oq1,
                            Ref<const BDS_Point> oq2,
                            Ref<const BDS_Point> oq3) const = 0;
    virtual ~BDS_SwapEdgeTest() = default;
};

class BDS_SwapEdgeTestRecover : public BDS_SwapEdgeTest {
public:
    BDS_SwapEdgeTestRecover();
    bool operator()(Ref<const BDS_Point> p1,
                    Ref<const BDS_Point> p2,
                    Ref<const BDS_Point> q1,
                    Ref<const BDS_Point> q2) const override;
    bool operator()(Ref<const BDS_Point> p1,
                    Ref<const BDS_Point> p2,
                    Ref<const BDS_Point> p3,
                    Ref<const BDS_Point> q1,
                    Ref<const BDS_Point> q2,
                    Ref<const BDS_Point> q3,
                    Ref<const BDS_Point> op1,
                    Ref<const BDS_Point> op2,
                    Ref<const BDS_Point> op3,
                    Ref<const BDS_Point> oq1,
                    Ref<const BDS_Point> oq2,
                    Ref<const BDS_Point> oq3) const override;
};

class BDS_SwapEdgeTestQuality : public BDS_SwapEdgeTest {
    bool test_quality_, test_small_triangles_;

public:
    BDS_SwapEdgeTestQuality(bool a, bool b = true);
    bool operator()(Ref<const BDS_Point> p1,
                    Ref<const BDS_Point> p2,
                    Ref<const BDS_Point> q1,
                    Ref<const BDS_Point> q2) const override;
    bool operator()(Ref<const BDS_Point> p1,
                    Ref<const BDS_Point> p2,
                    Ref<const BDS_Point> p3,
                    Ref<const BDS_Point> q1,
                    Ref<const BDS_Point> q2,
                    Ref<const BDS_Point> q3,
                    Ref<const BDS_Point> op1,
                    Ref<const BDS_Point> op2,
                    Ref<const BDS_Point> op3,
                    Ref<const BDS_Point> oq1,
                    Ref<const BDS_Point> oq2,
                    Ref<const BDS_Point> oq3) const override;
};

class BDS_SwapEdgeTestNormals : public BDS_SwapEdgeTest {
    GeomSurface * gf_;
    double ori_;

public:
    BDS_SwapEdgeTestNormals(GeomSurface * _gf, double ori);
    bool operator()(Ref<const BDS_Point> p1,
                    Ref<const BDS_Point> p2,
                    Ref<const BDS_Point> q1,
                    Ref<const BDS_Point> q2) const override;
    bool operator()(Ref<const BDS_Point> p1,
                    Ref<const BDS_Point> p2,
                    Ref<const BDS_Point> p3,
                    Ref<const BDS_Point> q1,
                    Ref<const BDS_Point> q2,
                    Ref<const BDS_Point> q3,
                    Ref<const BDS_Point> op1,
                    Ref<const BDS_Point> op2,
                    Ref<const BDS_Point> op3,
                    Ref<const BDS_Point> oq1,
                    Ref<const BDS_Point> oq2,
                    Ref<const BDS_Point> oq3) const override;
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

    [[nodiscard]] const std::map<int, Qtr<BDS_Point>> & points() const;
    [[nodiscard]] Span<const Qtr<BDS_Edge>> edges() const;
    [[nodiscard]] Span<const Qtr<BDS_Face>> triangles() const;
    // Points
    Ref<BDS_Point> add_point(int num, Point pt);
    Ref<BDS_Point> add_point(int num, UVParam uv, const GeomSurface * gf, BDS_GeomEntity ge);
    void del_point(Ref<BDS_Point> p);
    [[nodiscard]] Optional<Ref<BDS_Point>> find_point(int num) const;
    // Edges
    Optional<Ref<BDS_Edge>> add_edge(int p1, int p2);
    Ref<BDS_Edge> add_edge(Ref<BDS_Point> p1, Ref<BDS_Point> p2);
    void del_edge(Ref<BDS_Edge> e);
    [[nodiscard]] Optional<Ref<BDS_Edge>> find_edge(int p1, int p2) const;
    [[nodiscard]] Optional<Ref<BDS_Edge>> find_edge(Ref<BDS_Point> p1, Ref<BDS_Point> p2) const;
    [[nodiscard]] Optional<Ref<BDS_Edge>> find_edge(Ref<BDS_Point> p1, int p2) const;
    [[nodiscard]] Optional<Ref<BDS_Edge>>
    find_edge(Ref<BDS_Point> p1, Ref<BDS_Point> p2, Vtr<BDS_Face> t) const;
    // Triangles
    Optional<Ref<BDS_Face>>
    add_triangle(int p1, int p2, int p3, Optional<BDS_GeomEntity> ge = std::nullopt);
    Optional<Ref<BDS_Face>> add_triangle(Ref<BDS_Edge> e1,
                                         Ref<BDS_Edge> e2,
                                         Ref<BDS_Edge> e3,
                                         Optional<BDS_GeomEntity> ge = std::nullopt);
    void del_face(Vtr<BDS_Face> t);
    [[nodiscard]] Optional<Ref<BDS_Face>>
    find_triangle(Ref<BDS_Edge> e1, Ref<BDS_Edge> e2, Ref<BDS_Edge> e3) const;
    // Geom entities
    BDS_GeomEntity add_geom(int tag, int degree);
    // 2D operators
    Optional<Ref<BDS_Edge>> recover_edge(int p1,
                                         int p2,
                                         bool & fatal,
                                         std::set<EdgeToRecover> * e2r = nullptr,
                                         std::set<EdgeToRecover> * not_recovered = nullptr);
    Optional<Ref<BDS_Edge>> recover_edge_fast(Ref<BDS_Point> p1, Ref<BDS_Point> p2);

    /// Can invalidate the iterators for \p edge
    bool swap_edge(Ref<BDS_Edge>, const BDS_SwapEdgeTest & theTest, bool force = false);
    bool collapse_edge_parametric(Ref<BDS_Edge>, Ref<BDS_Point>, bool = false);
    bool smooth_point_centroid(Ref<BDS_Point> p, const GeomSurface & gf, double thresh);
    bool split_edge(Ref<BDS_Edge>, Ref<BDS_Point> mid, bool check_area_param = false);
    void cleanup();

private:
    int max_point_num_;
    std::set<BDS_GeomEntity, GeomLessThan> geom_;
    std::map<int, Qtr<BDS_Point>> points_;
    std::vector<Qtr<BDS_Edge>> edges_;
    std::vector<Qtr<BDS_Face>> triangles_;
};

void recur_tag(Ref<BDS_Face> t, BDS_GeomEntity g);

} // namespace krado
