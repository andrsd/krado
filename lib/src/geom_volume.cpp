// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_volume.h"
#include "krado/geom_surface.h"
#include "TopoDS.hxx"
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"
#include "TopExp_Explorer.hxx"

namespace krado {

GeomVolume::GeomVolume(const TopoDS_Solid & solid) : GeomShape(3, solid), solid_(solid)
{
    GProp_GProps props;
    BRepGProp::VolumeProperties(this->solid_, props);
    this->volume_ = props.Mass();
}

double
GeomVolume::volume() const
{
    return this->volume_;
}

std::vector<GeomSurface>
GeomVolume::surfaces() const
{
    std::vector<GeomSurface> surfs;
    TopExp_Explorer exp;
    for (exp.Init(this->solid_, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        auto gface = GeomSurface(face);
        surfs.emplace_back(gface);
    }
    return surfs;
}

GeomVolume::operator const TopoDS_Shape &() const
{
    return this->solid_;
}

} // namespace krado
