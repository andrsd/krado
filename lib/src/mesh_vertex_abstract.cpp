// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/mesh_vertex_abstract.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/exception.h"
#include "krado/mesh_vertex.h"
#include "krado/mesh_curve_vertex.h"
#include "krado/mesh_surface_vertex.h"
#include "krado/uv_param.h"
#include "krado/point.h"

namespace krado {

// Mesh vertex "serial" number
static int next_num = 0;

MeshVertexAbstract::MeshVertexAbstract(const GeomShape & geom_shape) :
    gid_(0),
    geom_shape_(geom_shape),
    num_(++next_num)
{
}

int
MeshVertexAbstract::global_id() const
{
    return this->gid_;
}

void
MeshVertexAbstract::set_global_id(int id)
{
    this->gid_ = id;
}

const GeomShape &
MeshVertexAbstract::geom_shape() const
{
    return this->geom_shape_;
}

int
MeshVertexAbstract::num() const
{
    return this->num_;
}

void
MeshVertexAbstract::set_num(int num)
{
    this->num_ = num;
}

//

std::tuple<double, bool>
reparam_mesh_vertex_on_curve(const MeshVertexAbstract * v, const GeomCurve & gcurve)
{
    double param = 1.e6;
    auto [t_lo, t_hi] = gcurve.param_range();
    bool ok = true;

#if 0
    if (gcurve.has_first_vertex() && gcurve.first_vertex()->mesh_vertices[0] == v)
        param = t_lo;
    else if (gcurve.has_last_vertex() && gcurve.last_vertex()->mesh_vertices[0] == v)
        param = t_hi;
    else {
        auto * cv = dynamic_cast<const MeshCurveVertex *>(v);
        param = cv->parameter();
    }
#endif

    if (!ok || param == 1.e6) {
        // Point pt(v->x(), v->y(), v->z());
        Point pt;
        param = gcurve.parameter_from_point(pt);
    }

    if (param < 1.e6)
        return { param, true };
    else
        return { 0., false };
}

/// Reparametrize the point onto the given surface
///
/// @param gsurface Geometric surface to reparametrize onto
/// @param gcurve Geometric curve
/// @param t Curve parameter
UVParam
reparam_on_surface(const GeomSurface & gsurface, const GeomCurve & gcurve, double t, int dir)
{
    const auto pt = gcurve.point(t);
    return gsurface.parameter_from_point(pt);
}

std::tuple<UVParam, bool>
reparam_mesh_vertex_on_surface(MeshVertexAbstract const * v,
                               const GeomSurface & geom_surface,
                               bool on_surface,
                               bool fail_on_seam,
                               int dir)
{
    auto * vtx = dynamic_cast<const MeshVertex *>(v);
    auto * curve_vtx = dynamic_cast<const MeshCurveVertex *>(v);
    auto * surface_vtx = dynamic_cast<const MeshSurfaceVertex *>(v);
    if (vtx) {
        auto param = geom_surface.parameter_from_point(v->point());
        return { param, true };
    }
    else if (curve_vtx) {
        auto & geom_curve = curve_vtx->geom_curve();
        if (fail_on_seam) {
            if (geom_curve.is_seam(geom_surface))
                return { { 0, 0 }, false };
        }
        auto t = curve_vtx->parameter();
        auto param = reparam_on_surface(geom_surface, geom_curve, t, dir);
        return { param, true };
    }
    else if (surface_vtx) {
        auto param = surface_vtx->parameter();
        return { param, true };
    }
    else
        throw Exception("Unsupported vertex type");
}

} // namespace krado
