// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/ops.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_shell.h"
#include "krado/geom_volume.h"
#include "krado/exception.h"
#include "Geom_TrimmedCurve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRep_Tool.hxx"
#include "BRepAlgo_NormalProjection.hxx"
#include "BRepFeat_SplitShape.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepAlgoAPI_Splitter.hxx"
#include "TopoDS_Edge.hxx"
#include "TopTools_DataMapOfShapeInteger.hxx"

namespace krado {

std::tuple<GeomCurve, GeomCurve>
split_curve(const GeomCurve & curve, Standard_Real split_param)
{
    double umin, umax;
    Handle(Geom_Curve) orig_curve = BRep_Tool::Curve(curve, umin, umax);
    if (split_param < umin || split_param > umax)
        throw Exception("Split parameter {} out of range [{}, {}]", split_param, umin, umax);

    Handle(Geom_Curve) first_part = new Geom_TrimmedCurve(orig_curve, umin, split_param);
    Handle(Geom_Curve) second_part = new Geom_TrimmedCurve(orig_curve, split_param, umax);

    auto first_edge = BRepBuilderAPI_MakeEdge(first_part);
    auto second_edge = BRepBuilderAPI_MakeEdge(second_part);

    return { GeomCurve(first_edge), GeomCurve(second_edge) };
}

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

GeomVolume
imprint(const GeomVolume & volume, const GeomVolume & imp_vol)
{
    TopTools_ListOfShape args;
    args.Append(volume);

    BRepAlgo_NormalProjection projection(volume);
    // arbitrary distance limit, shapes must be close together
    projection.SetMaxDistance(1e-10);
    TopExp_Explorer vol_exp;
    TopTools_DataMapOfShapeInteger crv_id;
    int id = 0;
    for (vol_exp.Init(imp_vol, TopAbs_EDGE); vol_exp.More(); vol_exp.Next()) {
        auto edge = TopoDS::Edge(vol_exp.Current());
        if (!crv_id.IsBound(edge)) {
            crv_id.Bind(edge, ++id);
            projection.Add(edge);
        }
    }
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Projection of volume onto volume failed.");
    TopTools_ListOfShape tools;
    projection.BuildWire(tools);
    if (tools.IsEmpty())
        throw Exception("Projection of volume onto volume yield empty result.");

    BRepAlgoAPI_Splitter splitter;
    splitter.SetArguments(args);
    splitter.SetTools(tools);
    splitter.Build();
    if (!splitter.IsDone())
        throw Exception("imprint volume with volume failed.");

    auto result = splitter.Shape();
    return GeomVolume(TopoDS::Solid(result));
}

} // namespace krado
