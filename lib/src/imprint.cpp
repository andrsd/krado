// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/imprint.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_shell.h"
#include "krado/geom_volume.h"
#include "krado/exception.h"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepAlgo_NormalProjection.hxx"
#include "BRepFeat_SplitShape.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
#include <TopExp_Explorer.hxx>

namespace krado {

GeomShell
imprint(const GeomSurface & surface, const GeomCurve & curve)
{
    BRepBuilderAPI_MakeWire wire_maker;
    auto edge = TopoDS::Edge(curve);
    wire_maker.Add(edge);
    const auto & projected_wire = wire_maker.Wire();

    auto face = TopoDS::Face(surface);
    BRepAlgo_NormalProjection projection(face);
    projection.Add(projected_wire);
    projection.Build();

    BRepFeat_SplitShape split_shape(face);
    split_shape.Add(projected_wire, face);
    split_shape.Build();
    if (split_shape.IsDone()) {
        auto split_surface = split_shape.Shape();
        if (split_surface.ShapeType() == TopAbs_SHELL)
            return GeomShell(TopoDS::Shell(split_surface));
        else
            throw Exception("Imprint did not produce a shell.");
    }
    else
        throw Exception("Imprint surface with curve failed.");
}

GeomVolume
imprint(const GeomVolume & volume, const GeomCurve & curve)
{
    auto edge = TopoDS::Edge(curve);
    BRepBuilderAPI_MakeWire wireMaker(edge);
    auto wire = wireMaker.Wire();

    auto solid = TopoDS::Solid(volume);
    BRepAlgo_NormalProjection projection(solid);
    projection.Add(wire);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto volume failed.");

    BRepFeat_SplitShape split_shape(solid);
    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());
    split_shape.Add(seq);

    split_shape.Build();
    if (!split_shape.IsDone())
        throw Exception("Imprint volume with curve failed.");

    auto result = split_shape.Shape();
    return GeomVolume(TopoDS::Solid(result));
}

} // namespace krado
