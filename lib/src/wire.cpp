// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/wire.h"
#include "krado/exception.h"
#include "BRepBuilderAPI_MakeWire.hxx"
#include "TopoDS.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"
#include "krado/geom_shape.h"

namespace krado {

Wire::Wire(const TopoDS_Wire & wire) : GeomShape(wire), wire_(wire) {}

double
Wire::length() const
{
    GProp_GProps props;
    BRepGProp::LinearProperties(this->wire_, props);
    return props.Mass();
}

Wire
Wire::create(const std::vector<GeomCurve> & curves)
{
    if (curves.size() == 0)
        throw Exception("The list of curves is empty");
    BRepBuilderAPI_MakeWire make_wire;
    for (auto & edge : curves)
        make_wire.Add(edge);
    make_wire.Build();
    if (!make_wire.IsDone())
        throw Exception("Wire was not created");
    return Wire(make_wire.Wire());
}

} // namespace krado
