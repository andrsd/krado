// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_volume.h"
#include "krado/geom_surface.h"
#include "krado/geom_model.h"
#include "krado/consts.h"
#include "TopoDS.hxx"
#include "TopExp_Explorer.hxx"

namespace krado {

GeomVolume::GeomVolume(const TopoDS_Solid & solid) : GeomShape(solid) {}

int
GeomVolume::dim() const
{
    return 3;
}

std::vector<GeomSurface>
GeomVolume::surfaces() const
{
    const auto & solid = TopoDS::Solid(this->shape_);
    std::vector<GeomSurface> surfs;
    TopExp_Explorer exp;
    for (exp.Init(solid, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        auto gface = GeomSurface(face);
        surfs.emplace_back(gface);
    }
    return surfs;
}

double
GeomVolume::mesh_size_at_param(Point /*pos*/) const
{
    return MAX_LC;
}

GeomVolume::operator const TopoDS_Solid &() const
{
    return TopoDS::Solid(this->shape_);
}

GeomVolume
GeomVolume::create(const GeomShape & shape)
{
    auto solid = TopoDS::Solid(shape);
    if (solid.IsNull())
        throw Exception("Failed to create geometrical volume from a shape");
    return GeomVolume(solid);
}

} // namespace krado
