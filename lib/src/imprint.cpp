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
#include "TopExp_Explorer.hxx"

namespace krado {

GeomShell
imprint(const GeomSurface & surface, const GeomCurve & curve)
{
    auto edge = TopoDS::Edge(curve);
    auto face = TopoDS::Face(surface);
    BRepAlgo_NormalProjection projection(face);
    projection.Add(edge);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto surface failed.");

    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());

    BRepFeat_SplitShape splitter(face);
    splitter.Add(seq);
    splitter.Build();
    if (splitter.IsDone()) {
        auto split_shape = splitter.Shape();
        if (split_shape.ShapeType() == TopAbs_SHELL)
            return GeomShell(TopoDS::Shell(split_shape));
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
    auto solid = TopoDS::Solid(volume);
    BRepAlgo_NormalProjection projection(solid);
    // arbitrary distance limit, shapes must be close together
    projection.SetMaxDistance(1e-10);
    projection.Add(edge);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto volume failed.");

    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());
    if (seq.Size() == 0)
        throw Exception("Imprint: projection of curve onto volume yield empty result.");

    BRepFeat_SplitShape splitter(solid);
    splitter.Add(seq);
    splitter.Build();
    if (!splitter.IsDone())
        throw Exception("Imprint volume with curve failed.");

    auto result = splitter.Shape();
    return GeomVolume(TopoDS::Solid(result));
}

} // namespace krado
