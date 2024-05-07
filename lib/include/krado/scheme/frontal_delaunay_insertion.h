// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// Original file: meshGFaceDelaunayInsertion.h

#pragma once

#include "krado/mesh_element.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_surface.h"
#include "krado/types.h"
#include "krado/uv_param.h"
#include "krado/geom_shape.h"
#include "krado/point.h"
#include <list>
#include <set>
#include <map>
#include <array>
#include <cassert>

namespace krado {

class BDS_Mesh;
class BDS_Point;

struct BidimMeshData {
    std::map<MeshVertexAbstract *, int> indices;
    std::vector<UVParam> uv;
    std::vector<double> v_sizes;
    std::vector<double> v_sizes_bgm;

    // std::vector<SMetric3> vMetricsBGM;
    std::map<MeshVertexAbstract *, MeshVertexAbstract *> * equivalence;
    std::map<MeshVertexAbstract *, UVParam> * parametric_coordinates;
    /// embedded edges
    std::set<MeshElement, MEdgeLessThan> internal_edges;
    /// embedded vertices
    std::set<MeshVertex *> internal_vertices;

    inline void
    add_vertex(MeshVertexAbstract * mv, UVParam uv, double size, double sizeBGM)
    {
        int index = this->uv.size();
        assert(mv != nullptr);
        if (mv->geom_shape().dim() == 2)
            mv->set_num(index);
        else
            this->indices[mv] = index;
        if (this->parametric_coordinates) {
            auto it = this->parametric_coordinates->find(mv);
            if (it != this->parametric_coordinates->end())
                uv = it->second;
        }
        this->uv.push_back(uv);
        this->v_sizes.push_back(size);
        this->v_sizes_bgm.push_back(sizeBGM);
    }

    inline int
    index(MeshVertexAbstract * mv) const
    {
        assert(mv != nullptr);
        if (mv->geom_shape().dim() == 2)
            return mv->num();
        return this->indices.at(mv);
    }

    inline MeshVertexAbstract *
    equivalent(MeshVertexAbstract * v1) const
    {
        if (this->equivalence) {
            auto it = this->equivalence->find(v1);
            if (it == this->equivalence->end())
                return nullptr;
            else
                return it->second;
        }
        else
            return nullptr;
    }

    explicit BidimMeshData(std::map<MeshVertexAbstract *, MeshVertexAbstract *> * e = nullptr,
                           std::map<MeshVertexAbstract *, UVParam> * p = nullptr) :
        equivalence(e),
        parametric_coordinates(p)
    {
    }
};

std::tuple<double, double, double> build_metric(const GeomSurface & geom_surface, UVParam uv);

bool in_circum_circle_aniso(const GeomSurface & surface,
                            UVParam p1,
                            UVParam p2,
                            UVParam p3,
                            UVParam uv,
                            std::tuple<double, double, double> metric);

class MTri3 {
private:
    bool deleted_;
    double circum_radius_;
    MeshElement base_;
    std::array<MTri3 *, 3> neigh_;

public:
    /// 2 is euclidian norm, -1 is infinite norm  , 3 quality
    static int radius_norm;

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

    inline double
    radius() const
    {
        return this->circum_radius_;
    }

    inline MeshVertexAbstract *
    other_side(int i) const
    {
        auto * n = this->neigh_[i];
        if (n == nullptr)
            return nullptr;
        auto * v1 = this->base_.vertex((i + 2) % 3);
        auto * v2 = this->base_.vertex(i);
        for (int j = 0; j < 3; ++j) {
            if (n->tri().vertex(j) != v1 && n->tri().vertex(j) != v2)
                return n->tri().vertex(j);
        }
        return nullptr;
    }

    MTri3(const MeshElement & t, double lc, BidimMeshData * data, MeshSurface * surface);

    inline void
    set_tri(const MeshElement & t)
    {
        this->base_ = t;
    }

    inline const MeshElement &
    tri() const
    {
        assert(this->base_.type() == ElementType::TRI3);
        return this->base_;
    }

    inline void
    set_neighbor(int iN, MTri3 * n)
    {
        this->neigh_[iN] = n;
    }

    inline MTri3 *
    neighbor(int iN) const
    {
        return this->neigh_[iN];
    }

    int in_circum_circle(const Point & p) const;

    inline int
    in_circum_circle(double x, double y) const
    {
        Point p(x, y, 0.);
        return in_circum_circle(p);
    }

    inline int
    in_circum_circle(const MeshVertex * v) const
    {
        return in_circum_circle(v->point());
    }

    inline void
    set_deleted(bool d)
    {
        this->deleted_ = d;
    }

    inline bool
    assert_neighbor() const
    {
        if (this->deleted_)
            return true;
        for (int i = 0; i < 3; ++i)
            if (this->neigh_[i] && (this->neigh_[i]->is_neighbor(this) == false))
                return false;
        return true;
    }

    inline bool
    is_neighbor(const MTri3 * t) const
    {
        for (int i = 0; i < 3; ++i)
            if (this->neigh_[i] == t)
                return true;
        return false;
    }

    class ComparePtr {
    public:
        inline bool
        operator()(const MTri3 * a, const MTri3 * b) const
        {
            assert(a != nullptr);
            assert(b != nullptr);
            if (a->radius() > b->radius())
                return true;
            else if (a->radius() < b->radius())
                return false;
            else
                return a < b;
        }
    };
};

void connect_triangles(std::list<MTri3 *> &);
void connect_triangles(std::vector<MTri3 *> &);
void connect_triangles(std::set<MTri3 *, MTri3::ComparePtr> & all_tris);

void
bowyer_watson_frontal(MeshSurface & surface,
                      std::map<MeshVertexAbstract *, MeshVertexAbstract *> * equivalence = nullptr,
                      std::map<MeshVertexAbstract *, UVParam> * parametric_coordinates = nullptr,
                      std::vector<UVParam> * true_boundary = nullptr);

struct EdgeXFace {
    MeshVertexAbstract * v[2];
    MTri3 * t1;
    int i1;
    int ori;

    EdgeXFace(MTri3 * t, int iFac) : t1(t), i1(iFac), ori(1)
    {
        assert(this->t1 != nullptr);
        this->v[0] = this->t1->tri().vertex(iFac == 0 ? 2 : iFac - 1);
        this->v[1] = this->t1->tri().vertex(iFac);
        assert(this->v[0] != nullptr);
        assert(this->v[1] != nullptr);
        if (this->v[0]->num() > this->v[1]->num()) {
            this->ori = -1;
            std::swap(this->v[0], this->v[1]);
        }
    }

    inline bool
    operator<(const EdgeXFace & other) const
    {
        assert(this->v[0] != nullptr);
        assert(this->v[1] != nullptr);
        assert(other.v[0] != nullptr);
        assert(other.v[1] != nullptr);
        if (this->v[0]->num() < other.v[0]->num())
            return true;
        if (this->v[0]->num() > other.v[0]->num())
            return false;
        if (this->v[1]->num() < other.v[1]->num())
            return true;
        return false;
    }

    inline bool
    operator==(const EdgeXFace & other) const
    {
        assert(this->v[0] != nullptr);
        assert(this->v[1] != nullptr);
        assert(other.v[0] != nullptr);
        assert(other.v[1] != nullptr);
        if (this->v[0]->num() == other.v[0]->num() && this->v[1]->num() == other.v[1]->num())
            return true;
        return false;
    }
};

} // namespace krado
