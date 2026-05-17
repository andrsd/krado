// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/prism.h"
#include "krado/exception.h"
#include "BRepPrimAPI_MakePrism.hxx"

namespace krado {

Prism::Prism(const TopoDS_Shape & shape) : GeomShape(shape) {}

Prism
Prism::create(const GeomShape & shape, Vector vec)
{
    gp_Vec gp_vec = vec;
    BRepPrimAPI_MakePrism prism(shape, gp_vec);
    prism.Build();
    if (!prism.IsDone())
        throw Exception("Prism was not created");
    return prism.Shape();
}

} // namespace krado
