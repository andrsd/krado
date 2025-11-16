// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/ops.h"
#include "krado/element.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_shell.h"
#include "krado/geom_volume.h"
#include "krado/exception.h"
#include "krado/log.h"
#include "krado/mesh.h"
#include "krado/types.h"
#include "Geom_TrimmedCurve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRep_Tool.hxx"
#include "BRepAlgoAPI_Fuse.hxx"
#include "BRepAlgoAPI_Cut.hxx"
#include "BRepAlgoAPI_Common.hxx"
#include "BRepAlgo_NormalProjection.hxx"
#include "BRepFeat_SplitShape.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepAlgoAPI_Splitter.hxx"
#include "TopoDS_Edge.hxx"
#include "TopTools_DataMapOfShapeInteger.hxx"
#include "krado/vector.h"

namespace krado {

std::tuple<GeomCurve, GeomCurve>
split_curve(const GeomCurve & curve, Standard_Real split_param)
{
    Log::info("Splitting curve {} at parameter u={}", curve.id(), split_param);

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
    Log::info("Imprinting curve {} onto surface {}", curve.id(), surface.id());

    BRepAlgo_NormalProjection projection(surface);
    projection.Add(curve);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto surface failed.");

    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());

    BRepFeat_SplitShape splitter(surface);
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
    Log::info("Imprinting volume {} with curve {}", volume.id(), curve.id());

    BRepAlgo_NormalProjection projection(volume);
    // arbitrary distance limit, shapes must be close together
    projection.SetMaxDistance(1e-10);
    projection.Add(curve);
    projection.Build();
    if (!projection.IsDone())
        throw Exception("Imprint: projection of curve onto volume failed.");

    TopTools_SequenceOfShape seq;
    TopExp_Explorer exp;
    for (exp.Init(projection.Projection(), TopAbs_EDGE); exp.More(); exp.Next())
        seq.Append(exp.Current());
    if (seq.Size() == 0)
        throw Exception("Imprint: projection of curve onto volume yield empty result.");

    BRepFeat_SplitShape splitter(volume);
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
    Log::info("Imprinting volume {} with volume {}", volume.id(), imp_vol.id());

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

std::map<marker_t, double>
compute_volume(const Mesh & mesh)
{
    auto element_volume = [&](const Element & elem) {
        switch (elem.type()) {
        case ElementType::LINE2: {
            auto idxs = elem.ids();
            auto vec = mesh.point(idxs[1]) - mesh.point(idxs[0]);
            return vec.magnitude();
        }

        case ElementType::TRI3: {
            auto idxs = elem.ids();
            auto va = mesh.point(idxs[1]) - mesh.point(idxs[0]);
            auto vb = mesh.point(idxs[2]) - mesh.point(idxs[0]);
            return 0.5 * cross_product(va, vb).magnitude();
        }

        case ElementType::TETRA4: {
            auto idxs = elem.ids();
            auto va = mesh.point(idxs[1]) - mesh.point(idxs[0]);
            auto vb = mesh.point(idxs[2]) - mesh.point(idxs[0]);
            auto vc = mesh.point(idxs[3]) - mesh.point(idxs[0]);
            return std::abs(dot_product(va, cross_product(vb, vc))) / 6.;
        }

        default:
            throw Exception("compute_volume: Unsupported element type {}", elem.type());
        }
    };

    auto cellsets_ids = mesh.cell_set_ids();
    if (cellsets_ids.empty()) {
        double volume = 0.;
        for (auto & elem : mesh.elements())
            volume += element_volume(elem);
        return { std::pair(0, volume) };
    }
    else {
        std::map<marker_t, double> vols_per_cellset;
        for (auto csid : cellsets_ids) {
            auto & volume = vols_per_cellset[csid];
            for (auto & cid : mesh.cell_set(csid)) {
                auto & elem = mesh.element(cid);
                volume += element_volume(elem);
            }
        }
        return vols_per_cellset;
    }
}

GeomShape
fuse(const GeomShape & shape, const GeomShape & tool, bool simplify)
{
    BRepAlgoAPI_Fuse alg(shape, tool);
    alg.Build();
    if (simplify)
        alg.SimplifyResult();
    if (!alg.IsDone())
        throw Exception("Objects were not fused");
    return GeomShape(alg.Shape());
}

GeomShape
fuse(const std::vector<GeomShape> & shapes, bool simplify)
{
    if (shapes.empty())
        throw Exception("No shapes to fuse");

    if (shapes.size() == 1)
        throw Exception("Only one shape provided");

    TopoDS_Shape sh = shapes[0];
    for (std::size_t i = 1; i < shapes.size(); ++i) {
        BRepAlgoAPI_Fuse alg(sh, shapes[i]);
        alg.Build();
        if (simplify)
            alg.SimplifyResult();
        if (!alg.IsDone())
            throw Exception("Objects were not fused");
        sh = alg.Shape();
    }

    return GeomShape(sh);
}

GeomShape
cut(const GeomShape & shape, const GeomShape & tool)
{
    BRepAlgoAPI_Cut alg(shape, tool);
    alg.Build();
    if (!alg.IsDone())
        throw Exception("Object was not cut");
    return GeomShape(alg.Shape());
}

GeomShape
intersect(const GeomShape & shape, const GeomShape & tool)
{
    BRepAlgoAPI_Common alg(shape, tool);
    alg.Build();
    if (!alg.IsDone())
        throw Exception("Object was not intersected");
    return GeomShape(alg.Shape());
}

} // namespace krado
