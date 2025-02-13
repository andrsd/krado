// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/imprint.h"
#include "krado/geom_curve.h"
#include "krado/geom_surface.h"
#include "krado/geom_shell.h"
#include "krado/exception.h"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepAlgo_NormalProjection.hxx"
#include "BRepFeat_SplitShape.hxx"
#include "TopoDS_Wire.hxx"
#include "TopoDS.hxx"

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

} // namespace krado
