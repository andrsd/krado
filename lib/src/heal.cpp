// SPDX-FileCopyrightText: 2026 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/heal.h"
#include "krado/geom_shape.h"
#include "krado/mesh_vertex_abstract.h"
#include "krado/timer.h"
#include "BRepLib.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "BRep_Tool.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"
#include "BRepCheck_Analyzer.hxx"
#include "BRepOffsetAPI_Sewing.hxx"
#include "ShapeBuild_ReShape.hxx"
#include "ShapeFix_Shape.hxx"
#include "ShapeFix_Wire.hxx"
#include "ShapeFix_Face.hxx"
#include "ShapeFix_FixSmallFace.hxx"
#include "ShapeFix_Wireframe.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"

namespace krado {

namespace {

TopoDS_Shape
remove_degenerated_edges(const TopoDS_Shape & shape)
{
    TopExp_Explorer exp;
    ShapeBuild_ReShape rebuild;
    for (exp.Init(shape, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());
        if (BRep_Tool::Degenerated(edge))
            rebuild.Remove(edge);
    }
    return rebuild.Apply(shape);
}

TopoDS_Shape
repair_faces(const TopoDS_Shape & shape)
{
    TopExp_Explorer exp;
    ShapeBuild_ReShape rebuild;
    for (exp.Init(shape, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());

        ShapeFix_Face sff(face);
        sff.FixAddNaturalBoundMode() = Standard_True;
        sff.FixSmallAreaWireMode() = Standard_True;
        sff.Perform();

        if (sff.Status(ShapeExtend_DONE1) || sff.Status(ShapeExtend_DONE2) ||
            sff.Status(ShapeExtend_DONE3) || sff.Status(ShapeExtend_DONE4) ||
            sff.Status(ShapeExtend_DONE5)) {
            TopoDS_Face newface = sff.Face();
            rebuild.Replace(face, newface);
        }
    }
    return rebuild.Apply(shape);
}

TopoDS_Shape
fix_degenerated(const TopoDS_Shape & shape)
{
    auto mod = remove_degenerated_edges(shape);
    mod = repair_faces(mod);
    return remove_degenerated_edges(mod);
}

TopoDS_Shape
remove_small_edges(const TopoDS_Shape & shape, double tolerance)
{
    TopExp_Explorer exp;
    ShapeBuild_ReShape rebuild;
    for (exp.Init(shape, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());
        GProp_GProps system;
        BRepGProp::LinearProperties(edge, system);
        if (system.Mass() < tolerance) {
            rebuild.Remove(edge);
        }
    }
    return rebuild.Apply(shape);
}

TopoDS_Shape
fix_small_wires(const TopoDS_Shape & shape, double tolerance)
{
    TopExp_Explorer exp0, exp1;
    ShapeBuild_ReShape rebuild;

    for (exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());

        for (exp1.Init(face, TopAbs_WIRE); exp1.More(); exp1.Next()) {
            TopoDS_Wire oldwire = TopoDS::Wire(exp1.Current());
            ShapeFix_Wire sfw(oldwire, face, tolerance);
            sfw.ModifyTopologyMode() = Standard_True;
            sfw.ClosedWireMode() = Standard_True;
            bool replace = false;
            replace = sfw.FixReorder() || replace;
            replace = sfw.FixConnected() || replace;

            if (sfw.FixSmall(Standard_False, tolerance) &&
                !(sfw.StatusSmall(ShapeExtend_FAIL1) || sfw.StatusSmall(ShapeExtend_FAIL2) ||
                  sfw.StatusSmall(ShapeExtend_FAIL3))) {
                replace = true;
            }

            replace = sfw.FixEdgeCurves() || replace;
            replace = sfw.FixDegenerated() || replace;
            replace = sfw.FixSelfIntersection() || replace;
            replace = sfw.FixLacking(Standard_True) || replace;
            if (replace) {
                TopoDS_Wire newwire = sfw.Wire();
                rebuild.Replace(oldwire, newwire);
            }
        }
    }
    return rebuild.Apply(shape);
}

TopoDS_Shape
fix_small_edges(const TopoDS_Shape & shape, double tolerance)
{
    auto s = shape;
    s = fix_small_wires(s, tolerance);
    s = remove_small_edges(s, tolerance);
    s = remove_degenerated_edges(s);

    ShapeFix_Wireframe sfwf;
    sfwf.SetPrecision(tolerance);
    sfwf.Load(s);
    sfwf.ModeDropSmallEdges() = Standard_True;
    sfwf.SetPrecision(tolerance);
    return sfwf.Shape();
}

TopoDS_Shape
fix_small_faces(const TopoDS_Shape & shape, double tolerance)
{
    ShapeFix_FixSmallFace sffsm;
    sffsm.Init(shape);
    sffsm.SetPrecision(tolerance);
    sffsm.Perform();
    return sffsm.FixShape();
}

TopoDS_Shape
sew_faces(const TopoDS_Shape & shape, double tolerance)
{
    TopExp_Explorer exp0;
    BRepOffsetAPI_Sewing sewed_obj(tolerance);

    for (exp0.Init(shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());
        sewed_obj.Add(face);
    }

    sewed_obj.Perform();

    if (!sewed_obj.SewedShape().IsNull())
        return sewed_obj.SewedShape();
    else
        return shape;
}

TopoDS_Shape
make_solids(const TopoDS_Shape & shape, double tolerance)
{
    TopoDS_Shape s = shape;
    TopExp_Explorer exp;
    BRepBuilderAPI_MakeSolid ms;
    int count = 0;
    for (exp.Init(shape, TopAbs_SHELL); exp.More(); exp.Next()) {
        count++;
        ms.Add(TopoDS::Shell(exp.Current()));
    }

    if (count > 0) {
        BRepCheck_Analyzer ba(ms);
        if (ba.IsValid()) {
            ShapeFix_Shape sfs;
            sfs.Init(ms);
            sfs.SetPrecision(tolerance);
            sfs.SetMaxTolerance(tolerance);
            sfs.Perform();
            s = sfs.Shape();
            for (exp.Init(s, TopAbs_SOLID); exp.More(); exp.Next()) {
                TopoDS_Solid solid = TopoDS::Solid(exp.Current());
                TopoDS_Solid new_solid = solid;
                BRepLib::OrientClosedSolid(new_solid);
                ShapeBuild_ReShape rebuild;
                rebuild.Replace(solid, new_solid);
                s = rebuild.Apply(s, TopAbs_COMPSOLID);
            }
        }
    }

    return s;
}

} // namespace

GeomShape
heal(const GeomShape & shape, double tolerance, Flags<HealFlag> flags)
{
    TopoDS_Shape s = shape;
    if (flags & FIX_DEGENERATED)
        s = fix_degenerated(s);
    if (flags & FIX_SMALL_EDGES)
        s = fix_small_edges(s, tolerance);
    if (flags & FIX_SMALL_FACES)
        s = fix_small_faces(s, tolerance);
    if (flags & SEW_FACES)
        s = sew_faces(s, tolerance);
    s = remove_degenerated_edges(s);
    if (flags & MAKE_SOLIDS)
        s = make_solids(s, tolerance);
    return GeomShape(s);
}

} // namespace krado
