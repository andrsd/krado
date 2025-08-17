// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "krado/mesh_vertex_abstract.h"
#include "krado/point.h"
#include "krado/geom_surface.h"
#include "krado/mesh_surface.h"
#include "krado/mesh_curve.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include <map>

namespace krado {

/// Maps local indices produced by a mesh generators to the mesh surface
class SurfaceIndexMapper {
public:
    SurfaceIndexMapper(Ptr<MeshSurface> surface) : surface_(surface)
    {
        for (auto & curve : surface->curves()) {
            for (auto & v : curve->bounding_vertices()) {
                auto pt = v->point();
                auto [it, inserted] = this->surf_idx_.try_emplace(pt, v);
                if (inserted)
                    surface->add_vertex(v);
            }
            for (auto & v : curve->curve_vertices()) {
                auto pt = v->point();
                auto [it, inserted] = this->surf_idx_.try_emplace(pt, v);
                if (inserted)
                    surface->add_vertex(v);
            }
        }

        for (int cidx = 0; cidx < surface->curves().size(); cidx++) {
            auto curve = surface->curves()[cidx];
            for (auto & v : curve->all_vertices()) {
                auto pt = v->point();
                this->curv_surf_idx_[cidx].try_emplace(pt, v);
            }
        }
        for (auto & v : surface->all_vertices()) {
            auto pt = v->point();
            this->surf_idx_.try_emplace(pt, v);
        }
    }

    [[nodiscard]] Ptr<MeshVertexAbstract>
    surface_vertex(double x, double y)
    {
        Point pt(x, y);
        auto it = this->surf_idx_.find(pt);
        if (it == this->surf_idx_.end()) {
            auto & geom_surface = surface_->geom_surface();
            auto uv = geom_surface.parameter_from_point(pt);
            auto sv = Ptr<MeshSurfaceVertex>::alloc(geom_surface, uv);
            this->surf_idx_.try_emplace(pt, sv);
            this->surface_->add_vertex(sv);
            return sv;
        }
        else
            return it->second;
    }

    [[nodiscard]] Ptr<MeshVertexAbstract>
    curve_vertex(int curve_idx, double x, double y)
    {
        auto & curv_map = this->curv_surf_idx_[curve_idx];
        Point pt(x, y);
        auto it = curv_map.find(pt);
        if (it == curv_map.end()) {
            auto curve = this->surface_->curves()[curve_idx];
            auto & geom_curve = curve->geom_curve();
            auto u = geom_curve.parameter_from_point(pt);
            auto cv = Ptr<MeshCurveVertex>::alloc(geom_curve, u);
            this->curv_surf_idx_[curve_idx].try_emplace(pt, cv);
            curve->add_vertex(cv);

            this->surf_idx_.try_emplace(pt, cv);
            this->surface_->add_vertex(cv);

            return cv;
        }
        else
            return it->second;
    }

private:
    Ptr<MeshSurface> surface_;
    /// Map from physical location to local surface vertex index
    std::map<Point, Ptr<MeshVertexAbstract>> surf_idx_;
    /// Map from physical location to local curve vertex index
    std::map<int, std::map<Point, Ptr<MeshVertexAbstract>>> curv_surf_idx_;
};

} // namespace krado
