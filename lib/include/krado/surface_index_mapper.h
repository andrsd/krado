#pragma once

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
    SurfaceIndexMapper(MeshSurface & surface) : surface(surface)
    {
        for (auto & curve : surface.curves()) {
            for (auto & v : curve->bounding_vertices()) {
                auto pt = v->point();
                auto idx = this->surf_idx.size();
                auto [it, inserted] = this->surf_idx.try_emplace(pt, idx);
                if (inserted)
                    surface.add_vertex(v);
            }
            for (auto & v : curve->curve_vertices()) {
                auto pt = v->point();
                auto idx = this->surf_idx.size();
                auto [it, inserted] = this->surf_idx.try_emplace(pt, idx);
                if (inserted)
                    surface.add_vertex(v);
            }
        }

        for (int cidx = 0; cidx < surface.curves().size(); cidx++) {
            auto curve = surface.curves()[cidx];
            for (int i = 0; i < curve->all_vertices().size(); i++) {
                auto pt = curve->all_vertices()[i]->point();
                this->curv_surf_idx[cidx].try_emplace(pt, i);
            }
        }
        for (int i = 0; i < surface.all_vertices().size(); i++) {
            auto pt = surface.all_vertices()[i]->point();
            this->surf_idx.try_emplace(pt, i);
        }
    }

    [[nodiscard]] int
    surface_idx(double x, double y)
    {
        auto pt = Point(x, y);
        auto it = this->surf_idx.find(pt);
        if (it == this->surf_idx.end()) {
            auto & geom_surface = surface.geom_surface();
            auto [u, v] = geom_surface.parameter_from_point(pt);
            auto sv = new MeshSurfaceVertex(geom_surface, u, v);
            auto idx = surface.all_vertices().size();
            this->surf_idx.try_emplace(pt, idx);
            this->surface.add_vertex(sv);
            return idx;
        }
        else
            return it->second;
    }

    [[nodiscard]] std::size_t
    curve_idx(int curve_idx, double x, double y)
    {
        auto & curv_map = this->curv_surf_idx[curve_idx];
        auto pt = Point(x, y);
        auto it = curv_map.find(pt);
        if (it == curv_map.end()) {
            auto curve = this->surface.curves()[curve_idx];
            auto & geom_curve = curve->geom_curve();
            auto u = geom_curve.parameter_from_point(pt);
            auto cv = new MeshCurveVertex(geom_curve, u);
            auto idx = curve->all_vertices().size();
            this->curv_surf_idx[curve_idx].try_emplace(pt, idx);
            curve->add_vertex(cv);

            auto sidx = surface.all_vertices().size();
            this->surf_idx.try_emplace(pt, sidx);
            this->surface.add_vertex(cv);

            return idx;
        }
        else
            return it->second;
    }

private:
    MeshSurface & surface;
    /// Map from physical location to local surface vertex index
    std::map<Point, int> surf_idx;
    /// Map from physical location to local curve vertex index
    std::map<int, std::map<Point, int>> curv_surf_idx;
};

} // namespace krado
