#include "krado/geom_shape.h"
#include "BRepLib.hxx"
#include "BRep_Tool.hxx"
#include "BRepTools.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "BRepCheck_Analyzer.hxx"
#include "BRepOffsetAPI_Sewing.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Edge.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"
#include "ShapeBuild_ReShape.hxx"
#include "ShapeFix_Shape.hxx"
#include "ShapeFix_Wire.hxx"
#include "ShapeFix_Face.hxx"
#include "ShapeFix_FixSmallFace.hxx"
#include "ShapeFix_Wireframe.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"

namespace krado {

GeomShape::GeomShape(const TopoDS_Shape & shape) : shape(shape) {}

void
GeomShape::clean()
{
    BRepTools::Clean(this->shape);
}

void
GeomShape::heal(double tolerance, Flags<HealFlag> flags)
{
    if (flags & FIX_DEGENERATED)
        fix_degenerated();
    if (flags & FIX_SMALL_EDGES)
        fix_small_edges(tolerance);
    if (flags & FIX_SMALL_FACES)
        fix_small_faces(tolerance);
    if (flags & SEW_FACES)
        sew_faces(tolerance);
    remove_degenerated_edges();
    if (flags & MAKE_SOLIDS)
        make_solids(tolerance);
}

void
GeomShape::scale(double scale_factor)
{
    gp_Trsf t;
    t.SetScaleFactor(scale_factor);
    BRepBuilderAPI_Transform trsf(this->shape, t);
    this->shape = trsf.Shape();
}

void
GeomShape::fix_degenerated()
{
    remove_degenerated_edges();
    repair_faces();
    remove_degenerated_edges();
}

void
GeomShape::remove_degenerated_edges()
{
    TopExp_Explorer exp;
    ShapeBuild_ReShape rebuild;
    for (exp.Init(this->shape, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());
        if (BRep_Tool::Degenerated(edge))
            rebuild.Remove(edge);
    }
    this->shape = rebuild.Apply(this->shape);
}

void
GeomShape::remove_small_edges(double tolerance)
{
    TopExp_Explorer exp;
    ShapeBuild_ReShape rebuild;
    for (exp.Init(this->shape, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());
        GProp_GProps system;
        BRepGProp::LinearProperties(edge, system);
        if (system.Mass() < tolerance) {
            rebuild.Remove(edge);
        }
    }
    this->shape = rebuild.Apply(this->shape);
}

void
GeomShape::repair_faces()
{
    TopExp_Explorer exp;
    ShapeBuild_ReShape rebuild;
    for (exp.Init(this->shape, TopAbs_FACE); exp.More(); exp.Next()) {
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
    this->shape = rebuild.Apply(this->shape);
}

void
GeomShape::fix_small_edges(double tolerance)
{
    fix_small_wires(tolerance);
    remove_small_edges(tolerance);
    remove_degenerated_edges();

    ShapeFix_Wireframe sfwf;
    sfwf.SetPrecision(tolerance);
    sfwf.Load(this->shape);
    sfwf.ModeDropSmallEdges() = Standard_True;
    sfwf.SetPrecision(tolerance);
    this->shape = sfwf.Shape();
}

void
GeomShape::fix_small_wires(double tolerance)
{
    TopExp_Explorer exp0, exp1;
    ShapeBuild_ReShape rebuild;

    for (exp0.Init(this->shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
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
    this->shape = rebuild.Apply(this->shape);
}

void
GeomShape::fix_small_faces(double tolerance)
{
    ShapeFix_FixSmallFace sffsm;
    sffsm.Init(this->shape);
    sffsm.SetPrecision(tolerance);
    sffsm.Perform();
    this->shape = sffsm.FixShape();
}

void
GeomShape::sew_faces(double tolerance)
{
    TopExp_Explorer exp0;
    BRepOffsetAPI_Sewing sewed_obj(tolerance);

    for (exp0.Init(this->shape, TopAbs_FACE); exp0.More(); exp0.Next()) {
        TopoDS_Face face = TopoDS::Face(exp0.Current());
        sewed_obj.Add(face);
    }

    sewed_obj.Perform();

    if (!sewed_obj.SewedShape().IsNull())
        this->shape = sewed_obj.SewedShape();
}

void
GeomShape::make_solids(double tolerance)
{
    TopExp_Explorer exp;
    BRepBuilderAPI_MakeSolid ms;
    int count = 0;
    for (exp.Init(this->shape, TopAbs_SHELL); exp.More(); exp.Next()) {
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
            this->shape = sfs.Shape();
            for (exp.Init(this->shape, TopAbs_SOLID); exp.More(); exp.Next()) {
                TopoDS_Solid solid = TopoDS::Solid(exp.Current());
                TopoDS_Solid new_solid = solid;
                BRepLib::OrientClosedSolid(new_solid);
                ShapeBuild_ReShape rebuild;
                rebuild.Replace(solid, new_solid);
                this->shape = rebuild.Apply(this->shape, TopAbs_COMPSOLID);
            }
        }
    }
}

GeomShape::operator const TopoDS_Shape &() const
{
    return this->shape;
}

} // namespace krado
