// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/geom_volume.h"
#include "TopoDS.hxx"
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"
#include "TopExp_Explorer.hxx"

namespace krado {

GeomVolume::GeomVolume(const TopoDS_Solid & solid) : solid(solid)
{
    GProp_GProps props;
    BRepGProp::VolumeProperties(this->solid, props);
    this->vol = props.Mass();
}

double
GeomVolume::volume() const
{
    return this->vol;
}

std::vector<GeomSurface>
GeomVolume::surfaces() const
{
    std::vector<GeomSurface> surfs;
    TopExp_Explorer exp;
    for (exp.Init(this->solid, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        auto gface = GeomSurface(face);
        surfs.emplace_back(gface);
    }
    return surfs;
}

GeomVolume::operator const TopoDS_Shape &() const
{
    return this->solid;
}

} // namespace krado
