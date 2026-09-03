// SPDX-FileCopyrightText: 1997-2024 C. Geuzaine, J.-F. Remacle
// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-2.0

#pragma once

#include "krado/types.h"
#include "krado/point.h"
#include "krado/uv_param.h"
#include <vector>
#include <map>
#include <set>
#include <ranges>

namespace krado {

class BDS_Edge;
class BDS_Face;
class BDS_Mesh;
class GeomSurface;
class GeomCurve;

struct BDSPointHandle {
    u32 id { UINT32_MAX };

    bool
    operator==(const BDSPointHandle & other) const
    {
        return this->id == other.id;
    }

    bool
    operator<(const BDSPointHandle & other) const
    {
        return this->id < other.id;
    }

    bool
    operator>(const BDSPointHandle & other) const
    {
        return this->id > other.id;
    }

    bool
    operator>=(const BDSPointHandle & other) const
    {
        return this->id >= other.id;
    }

    [[nodiscard]] bool
    is_null() const
    {
        return this->id == UINT32_MAX;
    }
};

struct BDSEdgeHandle {
    u32 id { UINT32_MAX };

    bool
    operator==(const BDSEdgeHandle & other) const
    {
        return this->id == other.id;
    }

    bool
    operator!=(const BDSEdgeHandle & other) const
    {
        return this->id != other.id;
    }

    [[nodiscard]] bool
    is_null() const
    {
        return this->id == UINT32_MAX;
    }
};

struct BDSFaceHandle {
    u32 id { UINT32_MAX };

    bool
    operator==(const BDSFaceHandle & other) const
    {
        return this->id == other.id;
    }

    [[nodiscard]] bool
    is_null() const
    {
        return this->id == UINT32_MAX;
    }
};

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

    [[nodiscard]] double u() const;
    [[nodiscard]] double v() const;
    void del(BDSEdgeHandle eh);

    // Characteristic length at point and is propagated
    double lc;
    Point point;
    UVParam uv;
    bool config_modified;
    u8 degenerated;
    i32 id;
    BDSPointHandle periodic_counterpart;
    Optional<BDS_GeomEntity> ge;
    std::vector<BDSEdgeHandle> edges;
};

class BDS_Edge {
public:
    BDS_Edge(BDSPointHandle A, BDSPointHandle B, Optional<BDS_GeomEntity> ge = std::nullopt);

    std::vector<BDSFaceHandle> faces;
    bool active;
    BDSPointHandle p1, p2;
    Optional<BDS_GeomEntity> ge;
};

class BDS_Face {
public:
    BDS_Face(BDSEdgeHandle A,
             BDSEdgeHandle B,
             BDSEdgeHandle C,
             Optional<BDS_GeomEntity> ge = std::nullopt);

    bool active;
    BDSEdgeHandle e1, e2, e3;
    Optional<BDS_GeomEntity> ge;
};

struct GeomLessThan {
    bool
    operator()(const BDS_GeomEntity & ent1, const BDS_GeomEntity & ent2) const
    {
        return ent1 < ent2;
    }
};

class BDS_SwapEdgeTest {
protected:
    const BDS_Mesh & mesh_;

    BDS_SwapEdgeTest(const BDS_Mesh & mesh) : mesh_(mesh) {}

public:
    virtual bool operator()(BDSPointHandle ph1,
                            BDSPointHandle ph2,
                            BDSPointHandle qh1,
                            BDSPointHandle qh2) const = 0;
    virtual bool operator()(BDSPointHandle ph1,
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
                            BDSPointHandle oqh3) const = 0;
    virtual ~BDS_SwapEdgeTest() = default;
};

class BDS_SwapEdgeTestRecover : public BDS_SwapEdgeTest {
public:
    BDS_SwapEdgeTestRecover(const BDS_Mesh & mesh);

    bool operator()(BDSPointHandle ph1,
                    BDSPointHandle ph2,
                    BDSPointHandle qh1,
                    BDSPointHandle qh2) const override;
    bool operator()(BDSPointHandle ph1,
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
                    BDSPointHandle oqh3) const override;
};

class BDS_SwapEdgeTestQuality : public BDS_SwapEdgeTest {
    bool test_quality_, test_small_triangles_;

public:
    BDS_SwapEdgeTestQuality(const BDS_Mesh & mesh, bool a, bool b = true);

    bool operator()(BDSPointHandle ph1,
                    BDSPointHandle ph2,
                    BDSPointHandle qh1,
                    BDSPointHandle qh2) const override;
    bool operator()(BDSPointHandle ph1,
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
                    BDSPointHandle oqh3) const override;
};

class BDS_SwapEdgeTestNormals : public BDS_SwapEdgeTest {
    GeomSurface * gf_;
    double ori_;

public:
    BDS_SwapEdgeTestNormals(const BDS_Mesh & mesh, GeomSurface * _gf, double ori);
    bool operator()(BDSPointHandle ph1,
                    BDSPointHandle ph2,
                    BDSPointHandle qh1,
                    BDSPointHandle qh2) const override;
    bool operator()(BDSPointHandle ph1,
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
                    BDSPointHandle oqh3) const override;
};

struct EdgeToRecover {
    EdgeToRecover(BDSPointHandle p1, BDSPointHandle p2);
    bool operator<(const EdgeToRecover & other) const;

private:
    BDSPointHandle p1_, p2_;
};

class BDS_Mesh {
public:
    BDS_Mesh();

    // Geom entities
    BDS_GeomEntity add_geom(int tag, int degree);

    BDSPointHandle add_point(int num, Point pt);
    BDSPointHandle
    add_point(int num, UVParam uv, const GeomSurface & geom_surface, BDS_GeomEntity ge);

    [[nodiscard]] const BDS_Point & get_point(BDSPointHandle pt) const;
    [[nodiscard]] BDS_Point & get_point(BDSPointHandle pt);

    [[nodiscard]] const BDS_Edge & get_edge(BDSEdgeHandle h) const;
    [[nodiscard]] BDS_Edge & get_edge(BDSEdgeHandle h);

    [[nodiscard]] const BDS_Face & get_face(BDSFaceHandle h) const;
    [[nodiscard]] BDS_Face & get_face(BDSFaceHandle h);

    [[nodiscard]] Optional<std::array<BDSPointHandle, 3>> get_nodes(BDSFaceHandle fh) const;

    [[nodiscard]] BDSPointHandle opposite_vertex(BDSEdgeHandle eh,
                                                 const std::array<BDSPointHandle, 3> & pts) const;

    [[nodiscard]] Optional<BDSPointHandle> opposite_vertex(BDSFaceHandle fh,
                                                           BDSEdgeHandle eh) const;

    [[nodiscard]] Optional<BDSPointHandle> common_vertex(BDSEdgeHandle eh1,
                                                         BDSEdgeHandle eh2) const;

    // Lightweight range generators returning strong handles
    [[nodiscard]] auto
    points() const
    {
        return std::views::iota(0u, static_cast<uint32_t>(this->points_.size())) |
               std::views::transform([](uint32_t id) { return BDSPointHandle { id }; });
    }

    [[nodiscard]] auto
    edges() const
    {
        return std::views::iota(0u, static_cast<uint32_t>(this->edges_.size())) |
               std::views::transform([](uint32_t id) { return BDSEdgeHandle { id }; });
    }

    [[nodiscard]] auto
    faces() const
    {
        return std::views::iota(0u, static_cast<uint32_t>(this->triangles_.size())) |
               std::views::transform([](uint32_t id) { return BDSFaceHandle { id }; });
    }

    // points

    [[nodiscard]] std::vector<BDSFaceHandle> triangles(BDSPointHandle ph) const;

    void set_lc(BDSPointHandle ph, double lc);

    [[nodiscard]] Optional<BDSPointHandle> find_point(int idx) const;

    void del_point(BDSPointHandle ph);

    // edges

    void set_ge(BDSEdgeHandle eh, Optional<BDS_GeomEntity> ge);

    [[nodiscard]] u32 num_faces(BDSEdgeHandle eh) const;

    Optional<BDSEdgeHandle> add_edge(BDSPointHandle ph1, BDSPointHandle ph2);

    [[nodiscard]] Optional<BDSEdgeHandle> find_edge(BDSPointHandle ph1, BDSPointHandle ph2) const;

    [[nodiscard]] Optional<BDSEdgeHandle>
    find_edge(BDSPointHandle ph1, BDSPointHandle ph2, BDSFaceHandle fh) const;

    [[nodiscard]] std::array<BDSPointHandle, 2> opposite_of(BDSEdgeHandle eh) const;

    [[nodiscard]] Optional<BDSPointHandle> other_vertex(BDSEdgeHandle eh, BDSPointHandle fh) const;

    [[nodiscard]] Optional<BDSFaceHandle> other_face(BDSEdgeHandle eh, BDSFaceHandle fh) const;

    [[nodiscard]] std::tuple<Optional<std::array<BDSPointHandle, 3>>,
                             Optional<std::array<BDSPointHandle, 3>>,
                             std::array<Optional<BDSPointHandle>, 2>>
    compute_neighborhood(BDSEdgeHandle eh) const;

    bool collapse_edge_parametric(BDSEdgeHandle eh, BDSPointHandle ph, bool = false);

    void del_edge(BDSEdgeHandle e);

    // faces

    Optional<BDSFaceHandle> add_face(BDSPointHandle p1,
                                     BDSPointHandle p2,
                                     BDSPointHandle p3,
                                     Optional<BDS_GeomEntity> ge = std::nullopt);

    Optional<BDSFaceHandle> add_face(BDSEdgeHandle e1,
                                     BDSEdgeHandle e2,
                                     BDSEdgeHandle e3,
                                     Optional<BDS_GeomEntity> ge = std::nullopt);

    Optional<BDSEdgeHandle> opposite_edge(BDSFaceHandle fh, BDSPointHandle p);

    void del_face(BDSFaceHandle th);

    //

    Optional<BDSEdgeHandle> recover_edge(BDSPointHandle p1,
                                         BDSPointHandle p2,
                                         bool & fatal,
                                         std::set<EdgeToRecover> * e2r = nullptr,
                                         std::set<EdgeToRecover> * not_recovered = nullptr);

    Optional<BDSEdgeHandle> recover_edge_fast(BDSPointHandle ph1, BDSPointHandle ph2);

    bool swap_edge(BDSEdgeHandle eh, const BDS_SwapEdgeTest & theTest, bool force = false);

    void recur_tag(BDSFaceHandle th, BDS_GeomEntity ge);

    void cleanup();

private:
    bool validity_of_cavity(UVParam p, const std::vector<BDSPointHandle> & nbg);

    Optional<std::vector<BDSPointHandle>>
    get_ordered_neighboring_vertices(BDSPointHandle p,
                                     const std::vector<BDSFaceHandle> & triangles);

    std::tuple<double, double> tutte_energy(Point pt, const std::vector<BDSPointHandle> & nbg);

    bool minimize_tutte_energy_proj(BDSPointHandle ph,
                                    double E_unmoved,
                                    const std::vector<BDSPointHandle> & nbg,
                                    const std::vector<UVParam> & kernel,
                                    const std::vector<double> & lc,
                                    const GeomSurface & gf);
    bool minimize_tutte_energy_param(BDSPointHandle ph,
                                     double E_unmoved,
                                     const std::vector<BDSPointHandle> & nbg,
                                     const std::vector<UVParam> & kernel,
                                     const std::vector<double> & lcs,
                                     const GeomSurface & gf);

    std::tuple<std::vector<UVParam>, std::vector<double>>
    compute_some_kind_of_kernel(BDSPointHandle ph, const std::vector<BDSPointHandle> & nbg);

    double surface_triangle_param(BDSPointHandle p1, BDSPointHandle p2, BDSPointHandle p3);

    std::set<BDS_GeomEntity, GeomLessThan> geom_;
    std::map<int, BDSPointHandle> points_by_num_;
    std::vector<BDS_Point> points_;
    std::vector<BDS_Edge> edges_;
    std::vector<BDS_Face> triangles_;
};

} // namespace krado
